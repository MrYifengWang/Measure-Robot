#include "pch.h"
#include "ThreadRtuSmacq.h"
#include "framework.h"
#include "MeasureBot.h"
#include <sstream>

/*
mode 0 – CW,
mode 1 – CW dose,
mode 2 – color (x/y),
mode 3 – color (u/v),
mode 4 – transmission,
mode 5 –reflection.
*/
#define Smacq_ms 50
ThreadRtuSmacq::ThreadRtuSmacq(MessageQueue* pq)
{
	pQueue = pq;
	pModbus = NULL;
}

ThreadRtuSmacq::~ThreadRtuSmacq(void)
{
	releaseSlave();
}
ThreadRtuSmacq* ThreadRtuSmacq::startThread(MessageQueue* pq) {
	ThreadRtuSmacq* thread = new ThreadRtuSmacq(pq);
	assert(thread);

	thread->setAutoDelete(true);
	thread->start();
	return thread;
}
bool ThreadRtuSmacq::onStart() {
	setupSlave();
	return true;
}

void ThreadRtuSmacq::handleMeasureConnect(Json::Value& command) {

	string comstr = command["port"].asString();
	int brate = command["baudrate"].asInt();
	int stop = command["stopbit"].asInt();
	int data = command["databit"].asInt();
	string check = command["check"].asString();

	int tempcyc = command["tempcycle"].asInt();
	int flowcyc = command["flowrate"].asInt();
	runCycle = tempcyc > flowcyc ? flowcyc : tempcyc;
	runCycle = runCycle < 500; runCycle = 500;
	initSlave((char*)comstr.c_str(), brate);
}
void ThreadRtuSmacq::handleMeasureReConnect(Json::Value& command) {}
void ThreadRtuSmacq::handleMeasureDisConnect(Json::Value& command) {}
void ThreadRtuSmacq::handleMeasureCheck(Json::Value& command) {}
int ThreadRtuSmacq::initSlave(char* com, int baudrate)
{
	// 步骤一：创建modbus RTU
	pModbus = modbus_new_rtu(com,baudrate,'E',8,1);
	if (!pModbus)
	{
		DebugLog::writeLogF("Failed to modbus_new_rtu");
		return -1;
	}
	// 步骤二: 485 RTU 模式
	modbus_rtu_set_serial_mode(pModbus, MODBUS_RTU_RS485);
	// 步骤三: 设置从机站号 1
	//modbus_set_slave(pModbus, addrid_);
	// 步骤四：设置超时时间 100 ms
	modbus_set_response_timeout(pModbus, 0, 100 * 1000);
	// 步骤五: 连接 (只是485和232只是打开串口，并未交互）

	Json::Value root;
	Json::FastWriter writer;
	root["cmd"] = "Connect";
	root["dev"] = "smacq";

	int ret = modbus_connect(pModbus);
	if (ret)
	{
		DebugLog::writeLogF("Failed to modbus_connect, ret =%d err:%s", ret, modbus_strerror(errno));
		modbus_free(pModbus);
		{
			root["code"] = -3001;
			string msg = writer.write(root);
			theApp.onMessageCallback(msg, SMACQ_MESSAGE);
		}
		return -2;
	}
	connected_ = 1;
	DebugLog::writeLogF("Succeed to modbus_new_rtu");
	root["code"] = 0;
	string msg = writer.write(root);
	theApp.onMessageCallback(msg, SMACQ_MESSAGE);

	return 0;
}


int ThreadRtuSmacq::setupSlave()
{
	//load config
	{
		SlaveRtu* slave = new SlaveRtu(2, this);
		slave->type_ = 2101;
		slave->sn_ = 20141;
		slaves_.push_back(slave);
	}
	{
		SlaveRtu* slave = new SlaveRtu(1, this);
		slave->type_ = 2003;
		slave->sn_ = 26517;
		slaves_.push_back(slave);
	}


	return 0;
}

int ThreadRtuSmacq::releaseSlave()
{
	for (int i = 0; i < slaves_.size(); i++)
	{
		delete slaves_[i];
		slaves_[i] = NULL;
	}
	return 0;
}


void ThreadRtuSmacq::run() {

	while (1)
	{
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
		if (connected_ != 1) continue;
		//if (automeasure_ != 1)continue;
		for (int i = 0; i < slaves_.size(); i++)
		{
			slaves_[i]->doMeasure();
			pts_time::wait(200);
		}
	}
}

void ThreadRtuSmacq::handleMessage(Json::Value& command)
{
	string cmd = command["cmd"].asString();

	if (cmd == "Measure") {
		handleMeasureOnce(command);
	}
	else if (cmd == "Exit") {
		return;
	}
	else if (cmd == "Automeasure") {
		automeasure_ = 1;
	}
	else if (cmd == "Stop") {
		automeasure_ = 0;
	}
	else if (cmd == "Check") {
		for (int i = 0; i < slaves_.size(); i++)
		{
			slaves_[i]->doCheck();
			pts_time::wait(200);
		}
	}
	else if (cmd == "Connect") {
		handleMeasureConnect(command);
	}

}


void ThreadRtuSmacq::handleMeasureOnce(Json::Value& command) {

	for (int i = 0; i < slaves_.size(); i++)
	{
		slaves_[i]->doMeasure();
		pts_time::wait(200);
	}
}


SlaveRtu::SlaveRtu(int addr, void* pp)
{
	pprent_ = (ThreadRtuSmacq*)pp;
	addrid_ = addr;
	type_ = 0;
	CHN_TEMP tmp;
	tmp.chn = 1;
	channels.push_back(tmp);
	

}
SlaveRtu::~SlaveRtu()
{

}

static void printHex(char* headinfo, unsigned char* pbuf, int size) {

	if (size <= 0) return;
	char buf[256] = { 0 };
	for (int i = 0; i < size; i++) {
		sprintf(buf + 2 * i, "%02X ", pbuf[i]);
	}

	DebugLog::writeLogF("====%s====\n%s", headinfo, buf);

}
static void printHex16(char* headinfo, unsigned short* pbuf, int size) {

	if (size <= 0) return;
	char buf[256] = { 0 };
	for (int i = 0; i < size; i++) {
		sprintf(buf + 4 * i, "%04X ", pbuf[i]);
	}

	DebugLog::writeLogF("====%s====\n%s", headinfo, buf);

}


#define chn_input_addr_temp 30100
#define chn_input_addr_flow 30000
#define chn_range_addr_flow 40101

int SlaveRtu::doCheck() {
	return 0;
}

/* range
0 5V 0
1 5V 1
0 20mA 2
4 20mA 3（默认
*/
int SlaveRtu::doMeasure() {

	Json::Value root;

	Json::FastWriter writer;
	root["cmd"] = "Measure";
	root["dev"] = "SMACQ";

	int ret = 0;

	if (!pprent_->connected_)
		return -1;
	ret = modbus_set_slave(pprent_->pModbus, addrid_);

	if (type_ / 100 == 21) {
		root["card"] = "2101";

		uint16_t buffer16t[10] = { 0x00 };

		for (int i = 0; i < channels.size(); i++) {
			pts_time::wait(Smacq_ms);
			memset(buffer16t, 0, 10);
			ret = modbus_read_input_registers(pprent_->pModbus, chn_input_addr_temp +channels[i].chn, 8, buffer16t);
			if (ret <= 0)
			{
				DebugLog::writeLogF("Failed to modbus_read_registers, temp =%d chn=%d", ret, channels[i].chn);
				return -4;
			}
			//printHex16("input_reg", buffer16t, 10);
			else {
				DebugLog::writeLogF("------M2101 temp=%d  [%f (oC)] ", buffer16t[0],float(buffer16t[0] / 10.0) ); //unit 0.1Co
				/*root["temp"] = buffer16t[0];
				root["temp_room"] = buffer16t[5];*/
				root["temp"] = buffer16t[5];
				root["temp_room"] = buffer16t[0];
			}
		}

	}
	else if (type_ / 100 == 20) {
		root["card"] = "2003";
		uint16_t buffer16t[10] = { 0x00 };

		for (int i = 0; i < channels.size(); i++) {
			pts_time::wait(Smacq_ms);

			memset(buffer16t, 0, 10);
			ret = modbus_read_input_registers(pprent_->pModbus, chn_input_addr_flow + channels[i].chn, 5, buffer16t);
			if (ret <= 0)
			{
				DebugLog::writeLogF("Failed to modbus_read_registers, ret =%d", ret);
				return -5;
			}
			//printHex16("input_reg", buffer16t, 10);
			else {
				DebugLog::writeLogF("------M2003 ch1=%d [%f],ch2=%d [%f],ch3=%d [%f],ch4=%d [%f],ch5=%d[%f]", 
					buffer16t[0], float(buffer16t[0]*100/65535.0 +0.3),
					buffer16t[1], float(buffer16t[1] * 100 / 65535.0 + 0.3),
					buffer16t[2], float(buffer16t[2] * 100 / 65535.0 + 0.3),
					buffer16t[3], float((buffer16t[3] * 100 / 65535.0)*40/100 ),
					buffer16t[4], float((buffer16t[4] * 100 / 65535.0))*16/100); //unit 0.1Co

				root["chn0"] = float(buffer16t[0] * 100 / 65535.0 + 0.3);
				root["chn1"] = float(buffer16t[1] * 100 / 65535.0 + 0.3);
				root["chn2"] = float(buffer16t[2] * 100 / 65535.0 + 0.3);
				root["chn3"] = float((buffer16t[3] * 100 / 65535.0) * 40 / 100);
				root["chn4"] = float((buffer16t[4] * 100 / 65535.0)) * 16 / 100;
			}
		}
	}

	string msg = writer.write(root);
	theApp.onMessage2RunBoard(msg, SMACQ_MESSAGE);
	return 0;
}