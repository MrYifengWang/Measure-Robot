#include "pch.h"
#include "ThreadRtuLx.h"
#include "framework.h"
#include "MeasureBot.h"
#include <sstream>

#define RTU_WAIT_MS 50

ThreadRtuLx::ThreadRtuLx(MessageQueue* pq)
{
	pQueue = pq;
}

ThreadRtuLx::~ThreadRtuLx(void)
{
	releaseSlave();
}
ThreadRtuLx* ThreadRtuLx::startThread(MessageQueue* pq) {
	ThreadRtuLx* thread = new ThreadRtuLx(pq);
	assert(thread);

	thread->setAutoDelete(true);
	thread->start();
	return thread;
}
char* us_Binary(unsigned short x) {
	static char buf[128] = {0};
	/*if (x > 1) {
		Print_Binary(x >> 1, tmpstr);
	}*/
	memset(buf, 0, 128);
	stringstream tmpstr("");
	for (int i = 15; i >= 0; i--) {
		tmpstr << ((x & 1<<i) ? "1" : "0");
	}

	sprintf(buf,"%s",tmpstr.str().c_str());

	//DebugLog::writeLogF("%s",buf);
	return buf;

}
char* ui_Binary(unsigned int x) {
	static char buf[128] = { 0 };
	/*if (x > 1) {
		Print_Binary(x >> 1, tmpstr);
	}*/
	stringstream tmpstr("");
	for (int i = 31; i >= 0; i--) {
		tmpstr << ((x & 1 << i) ? '1' : '0');
	}

	sprintf(buf, "%s", tmpstr.str().c_str());

	return buf;

}

bool ThreadRtuLx::onStart() {
	
	return true;
}

void ThreadRtuLx::handleMeasureConnect(Json::Value& command) {

	string comstr = command["port"].asString();
	int brate = command["baudrate"].asInt();
	int stop = command["stopbit"].asInt();
	int data = command["databit"].asInt();
	string check = command["check"].asString();
	runCycle = command["samplecycle"].asInt();
	
	if (runCycle < 1000)runCycle = 1000;
	initSlave((char*)comstr.c_str(), brate);
}
void ThreadRtuLx::handleMeasureReConnect(Json::Value& command) {}
void ThreadRtuLx::handleMeasureDisConnect(Json::Value& command) {}
void ThreadRtuLx::handleMeasureCheck(Json::Value& command) {}
int ThreadRtuLx::initSlave(char* com, int baudrate)
{
	// 步骤一：创建modbus RTU
/*pModbus = modbus_new_rtu("com5",
	115200,*/
	pModbus = modbus_new_rtu(com,
		baudrate,
		'E',
		8,
		1);
	if (!pModbus)
	{
		DebugLog::writeLogF("Failed to modbus_new_rtu");
		return -1;
	}
	// 步骤二: 485 RTU 模式
	modbus_rtu_set_serial_mode(pModbus, MODBUS_RTU_RS485);
	// 步骤三: 设置从机站号 1
	modbus_set_slave(pModbus, 1);
	// 步骤四：设置超时时间 100 ms
	modbus_set_response_timeout(pModbus, 0, 100 * 1000);
	// 步骤五: 连接 (只是485和232只是打开串口，并未交互）

	Json::Value root;
	Json::FastWriter writer;
	root["cmd"] = "Connect";
	root["dev"] = "lx";

	int ret = modbus_connect(pModbus);
	if (ret)
	{
		DebugLog::writeLogF("Failed to modbus_connect, ret =%d err:%s", ret, modbus_strerror(errno));
		modbus_free(pModbus);
		{
			root["code"] = -2001;
			string msg = writer.write(root);
			theApp.onMessageCallback(msg, L5000_MESSAGE);
		}
		return -2;
	}
	connected_ = 1;
	DebugLog::writeLogF("Succeed to modbus_new_rtu");
	{
		uint16_t buffer16t[10] = { 0x00 };
		int ret = 0;

		pts_time::wait(RTU_WAIT_MS);
		memset(buffer16t, 0, 10);
		ret = modbus_read_registers(pModbus, 0xD03A, 2, buffer16t);
		if (ret <= 0)
		{
			DebugLog::writeLogF("Failed to modbus_read_registers 2, ret =%d err=%d", ret, errno);
			//	return -5;
		}
		else {
			//DebugLog::writeLogF("------boxver:%d softver:%d ", buffer16t[0], buffer16t[1]); //unit 0.1Co
		}
	}

	root["code"] = 0;
	string msg = writer.write(root);
	theApp.onMessageCallback(msg, L5000_MESSAGE);

	return 0;
}

int ThreadRtuLx::releaseSlave()
{
	modbus_close(pModbus);
	modbus_free(pModbus);
	return 0;
}

void ThreadRtuLx::run() {
	int count = 0;
	while (1)
	{
		count++;
		Message* pmsg = pQueue->trypop(runCycle);
		if (pmsg != NULL) {
			DebugLog::writeLogF(pmsg->strData_.c_str());

			Json::Reader reader;
			Json::Value jmessage;
			if (reader.parse(pmsg->strData_, jmessage))
			{
				if (jmessage["cmd"].asString() == "Exit")
					break;
				handleMessage(jmessage);
			}

			delete pmsg;
		}
		doMeasure();
		if (count == 5) {
			count = 0;
			checkStat();
		}
	}
}

int NumberOf1(int n) {
	int count = 0;
	while (n != 0) {
		count++;
		n = n & (n - 1);
	}
	return count;
}

void ThreadRtuLx::handleMessage(Json::Value& command)
{
	string cmd = command["cmd"].asString();

	if (cmd != "Connect") {
		if (!connected_)
			return;
	}

	if (cmd == "Channel") {
		handleChannelTurn(command);
	}
	else if (cmd == "Switch") {
		handleTurn(command);
	}
	else if (cmd == "setChannel") {
		handleChannelSetChannel(command);
	}
	else if (cmd == "setRatio") {
		handleChannelSetRatio(command);
	}
	else if (cmd == "Exit") {
		return;
		//handleMeasureSet(command);
	}
	else if (cmd == "Connect") {
		handleMeasureConnect(command);
	}
	else if (cmd == "DisConnect") {
		handleMeasureDisConnect(command);
	}
	else if (cmd == "Check") {
		handleMeasureCheck(command);
	}

}
void ThreadRtuLx::handleChannelSetRatio(Json::Value& command) {
	int ratio = command["val"].asInt();
	ratio = ratio *10;
	pts_time::wait(RTU_WAIT_MS);
	int ret=modbus_write_register(pModbus, 0x1001, ratio);
	if (ret <= 0)
	{
		DebugLog::writeLogF("Failed to modbus_read_registers ratio 1, ret =%d err=%d", ret, errno);
		//	return -5;
	}
	else {
		DebugLog::writeLogF("------set ratio:%d", ratio); //unit 0.1Co
	}
}


#define D3_addr_power (0x1000)
void ThreadRtuLx::handleTurn(Json::Value& command)
{
	pts_time::wait(RTU_WAIT_MS);
	int ret = -10;
	string stat = command["enable"].asString();
	if (stat == "on") {
		ret = modbus_write_register(pModbus, D3_addr_power, 1);
	}
	else if (stat == "off") {
		ret = modbus_write_register(pModbus, D3_addr_power, 2);
	}
	else if (stat == "reset") {
		ret = modbus_write_register(pModbus, D3_addr_power, 3);
	}

	DebugLog::writeLogF("==== power switch ret=%d",ret);
}

#define addr_chn_power (0xD046+1)
#define addr_chn_vol (0xD100)
#define addr_chn_temper (0xD200+1)

int ThreadRtuLx::checkStat() {
	Json::Value root;
	Json::FastWriter writer;
	root["cmd"] = "Stat";
	root["dev"] = "L5000";

	if (connected_ != 1)
		return -1;

	uint16_t buffer16t[10] = { 0x00 };
	int ret = 0;
	pts_time::wait(RTU_WAIT_MS);
	memset(buffer16t, 0, 10);
	ret = modbus_read_registers(pModbus, 0xD000 + 1, 5, buffer16t);
	if (ret <= 0)
	{
		DebugLog::writeLogF("Failed to modbus_read_registers 1, ret =%d err=%d", ret, errno);
		//	return -5;
	}
	else {
		//DebugLog::writeLogF("------power:%d cur:%d stat:%d err:%d warn:%d",0, buffer16t[0], buffer16t[1], buffer16t[2], buffer16t[3]); //unit 0.1Co

		root["pow"] = buffer16t[0];
		root["cur"] = buffer16t[1];
		root["stat"] = buffer16t[2];
		root["err"] = buffer16t[3];
		root["warn"] = buffer16t[4];

	}

	pts_time::wait(RTU_WAIT_MS);
	memset(buffer16t, 0, 10);
	ret = modbus_read_registers(pModbus, 0xD012, 1, buffer16t);
	if (ret > 0) {
		DebugLog::writeLogF("------chn selected:%u %s", buffer16t[0], us_Binary(buffer16t[0])); //unit 0.1Co
		root["chns"] = buffer16t[0];
	}
	string msg = writer.write(root);
	theApp.onMessageCallback(msg, L5000_MESSAGE);

}

int ThreadRtuLx::doMeasure() {

	Json::Value root;
	Json::Value arr;
	Json::Value arr1;

	Json::FastWriter writer;
	root["cmd"] = "Measure";
	root["dev"] = "L5000";

	if (connected_ != 1)
		return -1;

	uint16_t buffer16t[10] = { 0x00 };
	int ret = 0;

	memset(buffer16t, 0, 10);
	
	int i = 0;
	do {
		if (channelBits & 1 << i) {
			pts_time::wait(RTU_WAIT_MS);
			memset(buffer16t, 0, 10);
			ret = modbus_read_registers(pModbus, addr_chn_temper + i, 10, buffer16t);
			if (ret <= 0)
			{
				DebugLog::writeLogF("Failed to modbus_read_registers temp 3, ret =%d", ret);
				//	return -5;
			}
			else {
				stringstream tmpstr("");

				for (int j = 0; j < 10; j++) {
					if (channelBits & 1 << (i + j)) {
						tmpstr << "chn:" << i+j+1;
						tmpstr << " temp:" << buffer16t[j];
						tmpstr << "\t";

						Json::Value item;
						item["chn"] = i + j + 1;
						item["temp"] = buffer16t[j];
						arr.append(item);
					}
				}
				//DebugLog::writeLogF("%s", tmpstr.str().c_str()); //unit 0.1Co
			}
			i += 10;
		}
		else {
			i++;
		}

	} while (i < 32);

	i = 0;
	do {
		if (channelBits & 1 << i) {
			pts_time::wait(RTU_WAIT_MS);
			memset(buffer16t, 0, 10);
			ret = modbus_read_registers(pModbus, addr_chn_vol + i*3, 3*3, buffer16t);
			if (ret <= 0)
			{
				DebugLog::writeLogF("Failed to modbus_read_registers 4, ret =%d", ret);
				//return -5;
			}
			else {
				stringstream tmpstr("");
				for (int j = 0; j < 3; j++) {
					if (channelBits & 1 << (i + j)) {
						tmpstr << "chn:" << i+j+1;
					/*	tmpstr << " vol:" << buffer16t[j*3 + 0];
						tmpstr << " cur:" << buffer16t[j*3 + 1];
						tmpstr << " pow:" << buffer16t[j*3 + 2];*/

						tmpstr << " vol:" << buffer16t[j * 3 + 1];
						tmpstr << " cur:" << buffer16t[j * 3 + 2];
						tmpstr << " pow:" << buffer16t[j * 3 + 0]; 
						tmpstr << "\t";

						Json::Value item;
						item["chn"] = i + j + 1;
						/*item["vol"] = buffer16t[j * 3 + 0];
						item["cur"] = buffer16t[j * 3 + 1];
						item["pow"] = buffer16t[j * 3 + 2];*/

						item["vol"] = buffer16t[j * 3 + 1];
						item["cur"] = buffer16t[j * 3 + 2];
						item["pow"] = buffer16t[j * 3 + 0]; 

						arr1.append(item);
					}
				}
				//DebugLog::writeLogF("addr:%d %s", addr_chn_vol + i * 3, tmpstr.str().c_str()); //unit 0.1Co
			}
			i += 3;
		}
		else {
			i++;
		}

	/*	pts_time::wait(RTU_WAIT_MS);
		memset(buffer16t, 0, 10);
		ret = modbus_read_registers(pModbus, 53541, 3 * 1, buffer16t);

		DebugLog::writeLogF("=====chn 13:%d  %d  %d", buffer16t[0], buffer16t[1], buffer16t[2]);*/

	} while (i < 32);

	root["temps"] = arr;
	root["pows"] = arr1;

	string msg = writer.write(root);
	theApp.onMessage2RunBoard(msg, L5000_MESSAGE);

	
	/*
	for (int i = 0; i < channels.size(); i++) {
		pts_time::wait(500);
	//	memset(buffer16t, 0, 10);

	//	ret = modbus_read_registers(pModbus, addr_chn_power + channels[i].chn, 1, buffer16t);
	//	if (ret <= 0)
	//	{
	//		DebugLog::writeLogF("Failed to modbus_read_registers 2, ret =%d ,chn=%d", ret, channels[i].chn);
	////		return -5;
	//	}
	//	else {
	//		DebugLog::writeLogF("------chn %d  total power= %d [0-5V]", channels[i].chn, buffer16t[0]); //unit 0.1Co
	//	}

	//	pts_time::wait(500);


		memset(buffer16t, 0, 10);
		ret = modbus_read_registers(pModbus, addr_chn_temper + channels[i].chn, 1, buffer16t);
		if (ret <= 0)
		{
			DebugLog::writeLogF("Failed to modbus_read_registers 3, ret =%d", ret);
		//	return -5;
		}
		else {
			DebugLog::writeLogF("------chn %d  temper= %d [0-5V]", channels[i].chn, buffer16t[0]); //unit 0.1Co
		}
		pts_time::wait(500);

		memset(buffer16t, 0, 10);
		ret = modbus_read_registers(pModbus, addr_chn_vol + channels[i].chn, 3, buffer16t);
		if (ret <= 0)
		{
			DebugLog::writeLogF("Failed to modbus_read_registers 4, ret =%d", ret);
			//return -5;
		}
		else {
			DebugLog::writeLogF("------chn %d vol = %d  cur = %d  power=%d[0-5V]", channels[i].chn, buffer16t[0], buffer16t[1], buffer16t[2]); //unit 0.1Co
		}


	}
	*/

}

void ThreadRtuLx::handleChannelTurn(Json::Value& command) {

	DebugLog::writeLogF("-------did'nt use now!");
	return;
	uint16_t val1 = 0;
	uint16_t val2 = 0;
	uint16_t val3 = 0;

	modbus_read_registers(pModbus, 0x2000, 1, &val1);
	modbus_read_registers(pModbus, 0x2001, 1, &val2);
	modbus_read_registers(pModbus, 0x2002, 1, &val3);

	string stat = command["stat"].asString();
	Json::Value arr = command["channels"];
	if (stat == "on") {

		for (int i = 0; i < arr.size(); i++) {
			int chn = arr[i].asInt();
			if (chn <= 16) {
				val1 |= (1 << (chn - 1));
			}
			else if (chn > 16 && chn <= 32) {
				val2 |= (1 << (chn - 1 - 16));
			}
			else if (chn > 32) {
				val3 |= (1 << (chn - 1 - 32));
			}
		}
	}
	else {
		for (int i = 0; i < arr.size(); i++) {
			int chn = arr[i].asInt();
			if (chn <= 16) {
				val1 &= ~(1 << (chn - 1));
			}
			else if (chn > 16 && chn <= 32) {
				val2 &= ~(1 << (chn - 1 - 16));
			}
			else if (chn > 32) {
				val3 &= ~(1 << (chn - 1 - 32));
			}
		}
	}

	channelOnBits = val2 << 16 + val1;
	modbus_write_register(pModbus, 0x2000, val1);
	modbus_write_register(pModbus, 0x2001, val2);
	modbus_write_register(pModbus, 0x2002, val3);
}

#define addr_enable 0x1002
//#define addr_enable 0x2000
void ThreadRtuLx::handleChannelSetChannel(Json::Value& command) {
	uint16_t val1 = 0;
	uint16_t val2 = 0;
	uint16_t val3 = 0;
	pts_time::wait(RTU_WAIT_MS);
	modbus_read_registers(pModbus, addr_enable, 1, &val1);


	//modbus_read_registers(pModbus, 0xD012, 1, &val1);
	
	//modbus_read_registers(pModbus, addr_enable +1, 1, &val2);
	//modbus_read_registers(pModbus, addr_enable +2, 1, &val3);

//	DebugLog::writeLogF("ori chn:%s", us_Binary(val3));
//	DebugLog::writeLogF("ori chn:%s", us_Binary(val2));
	DebugLog::writeLogF("ori chn %d:%s", val1, us_Binary(val1));

	string stat = command["enable"].asString();
	Json::Value arr = command["channels"];
	if (stat == "on") {

		for (int i = 0; i < arr.size(); i++) {
			int chn = arr[i].asInt();
			if (chn <= 16) {
				val1 |= (1 << (chn - 1));
			}
			else if (chn > 16 && chn <= 32) {
				val2 |= (1 << (chn - 1 - 16));
			}
			else if (chn > 32) {
				val3 |= (1 << (chn - 1 - 32));
			}
		}
	}
	else {
		for (int i = 0; i < arr.size(); i++) {
			int chn = arr[i].asInt();
			if (chn <= 16) {
				val1 &= ~(1 << (chn - 1));
			}
			else if (chn > 16 && chn <= 32) {
				val2 &= ~(1 << (chn - 1 - 16));
			}
			else if (chn > 32) {
				val3 &= ~(1 << (chn - 1 - 32));
			}
		}
	}
	
//	DebugLog::writeLogF("new chn:%s", us_Binary(val3));
//	DebugLog::writeLogF("new chn:%s", us_Binary(val2));
	DebugLog::writeLogF("---------new chn %d :%s\n", val1, us_Binary(val1));

	channelBits |= val2 << 15;
	channelBits |= val1;

	pts_time::wait(RTU_WAIT_MS);

	int ret1 = modbus_write_register(pModbus, addr_enable, val1);
	if (ret1 <= 0) {
		DebugLog::writeLogF("=========== write chn ==========%s", us_Binary(val1));
	}
	//modbus_write_register(pModbus, addr_enable+1, val2);
	//modbus_write_register(pModbus, addr_enable+2, val3);
}

void ThreadRtuLx::handleChannelInitChannel(Json::Value& command) {
	uint16_t val1 = 0;
	uint16_t val2 = 0;
	uint16_t val3 = 0;
	pts_time::wait(RTU_WAIT_MS);
	modbus_read_registers(pModbus, addr_enable, 1, &val1);


	//modbus_read_registers(pModbus, 0xD012, 1, &val1);

	//modbus_read_registers(pModbus, addr_enable +1, 1, &val2);
	//modbus_read_registers(pModbus, addr_enable +2, 1, &val3);

//	DebugLog::writeLogF("ori chn:%s", us_Binary(val3));
//	DebugLog::writeLogF("ori chn:%s", us_Binary(val2));
	DebugLog::writeLogF("ori chn %d:%s", val1, us_Binary(val1));

	string stat = command["enable"].asString();
	Json::Value arr = command["channels"];
	if (stat == "on") {

		for (int i = 0; i < arr.size(); i++) {
			int chn = arr[i].asInt();
			if (chn <= 16) {
				val1 |= (1 << (chn - 1));
			}
			else if (chn > 16 && chn <= 32) {
				val2 |= (1 << (chn - 1 - 16));
			}
			else if (chn > 32) {
				val3 |= (1 << (chn - 1 - 32));
			}
		}
	}
	else {
		for (int i = 0; i < arr.size(); i++) {
			int chn = arr[i].asInt();
			if (chn <= 16) {
				val1 &= ~(1 << (chn - 1));
			}
			else if (chn > 16 && chn <= 32) {
				val2 &= ~(1 << (chn - 1 - 16));
			}
			else if (chn > 32) {
				val3 &= ~(1 << (chn - 1 - 32));
			}
		}
	}

	//	DebugLog::writeLogF("new chn:%s", us_Binary(val3));
	//	DebugLog::writeLogF("new chn:%s", us_Binary(val2));
	DebugLog::writeLogF("---------new chn %d :%s\n", val1, us_Binary(val1));

	channelBits |= val2 << 15;
	channelBits |= val1;

	pts_time::wait(RTU_WAIT_MS);

	int ret1 = modbus_write_register(pModbus, addr_enable, val1);
	if (ret1 <= 0) {
		DebugLog::writeLogF("=========== write chn ==========%s", us_Binary(val1));
	}
	//modbus_write_register(pModbus, addr_enable+1, val2);
	//modbus_write_register(pModbus, addr_enable+2, val3);
}


