#include "pch.h"
#include "ThreadX20.h"
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
ThreadX20::ThreadX20(MessageQueue* pq)
{
	pQueue = pq;
	handle = 0;
	curChannel = 1;
}

ThreadX20::~ThreadX20(void)
{
	releaseSdk();
}
ThreadX20* ThreadX20::startThread(MessageQueue* pq) {
	ThreadX20* thread = new ThreadX20(pq);
	assert(thread);

	thread->setAutoDelete(true);
	thread->start();
	return thread;
}
bool ThreadX20::onStart() {
	return true;
}
void ThreadX20::handleMeasureConnect(Json::Value& command) {
	initSdk(command);
}
void ThreadX20::handleMeasureReConnect(Json::Value& command) {}
void ThreadX20::handleMeasureDisConnect(Json::Value& command) {}
void ThreadX20::handleMeasureCheck(Json::Value& command) {}


void ThreadX20::run() {

	while (1)
	{
		Message* pmsg = pQueue->trypop(runCyc);
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
	}
}

void ThreadX20::handleMessage(Json::Value& command)
{
	string cmd = command["cmd"].asString();
	if (cmd != "Connect") {
		if (!connected_)
			return;
	}
	if (cmd == "DevInfo") {
		handleShowDevinfo(command);
	}
	else if (cmd == "Measure") {
		handleMeasureOnce(command);
	}
	else if (cmd == "AutoMeasure") {
		handleMeasureAuto(command);
	}
	else if (cmd == "GetValue") {
		handleMeasureOnce(command);
	}
	else if (cmd == "Set") {
		handleMeasureSet(command);
	}
	else if (cmd == "Stop") {
		handleMeasureStop(command);
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


void ThreadX20::handleMeasureOnce(Json::Value& command) {

	Json::Value root;
	Json::FastWriter writer;
	root["cmd"] = "Measure";
	root["dev"] = "x20";

	double cwval = 0;
	int unitNumber;
	char tmbuf[256] = { 0 };
	char* unitstr = tmbuf;
	int ret = GOMDX20_measureCW(handle, curChannel);
	if (ret == 0) {
		GOMDX20_getCWValue(handle, curChannel, &cwval);
		GOMDX20_getChannelUnitNumber(handle, curChannel, &unitNumber);
		GOMDX20_getUnitString(handle, unitNumber, false, unitstr);
		root["tm"] = pts_time::current();
		root["val"] = cwval;
		root["unit"] = unitstr;

	}
	//DebugLog::writeLogF("measure cw ret = %d val=%lf", ret, cwval);

	string msg = writer.write(root);
	theApp.onMessageCallback(msg, X20_MESSAGE);


}
void ThreadX20::handleMeasureAuto(Json::Value& command) {

	Json::Value root;
	Json::FastWriter writer;
	root["cmd"] = "AutoMeasure";
	root["dev"] = "x20";
	root["code"] = 0;


	double cwval = 0;
	int ret;
	bool isactive = true;

	int unitNumber;
	char tmbuf[256] = { 0 };
	char* unitstr = tmbuf;
	
	while (1) {
		ret = GOMDX20_measureCW(handle, curChannel);
		if (ret >= 0) {
			GOMDX20_getCWValue(handle, curChannel, &cwval);
			GOMDX20_getChannelUnitNumber(handle, curChannel, &unitNumber);
			GOMDX20_getUnitString(handle, unitNumber, false, unitstr);
			root["tm"] = (double)pts_time::currentms();
			root["val"] = cwval;
			root["unit"] = unitstr;
			root["unit_idx"] = unitNumber;

			string msg = writer.write(root);
			theApp.onMessageCallback(msg, X20_MESSAGE);

		//	DebugLog::writeLogF("measure cw : tm=%ld ,ret = %d,unit=%d [val=%lf %s]", pts_time::currentms(), ret, unitNumber, cwval, unitstr);
		}
		else {
			DebugLog::writeLogF("measure cw failed!");
		}

		int idx = pQueue->peek("auto_stop");
		if (idx >= 0) {
			break;
		}
	}


}
void ThreadX20::handleMeasureStop(Json::Value& command)
{

}

void ThreadX20::handleMeasureSet(Json::Value& command) {

	Json::Value root;
	Json::FastWriter writer;
	root["cmd"] = "Set";
	root["dev"] = "x20";

	int ret = 0;

	string wavelength = command["wavelength"].asString();
	string range = command["range"].asString();
	int intetime = command["intetime"].asInt();

	ret = GOMDX20_setIntegrationTimeInMs(handle, 0, intetime);
	ret += GOMDX20_setIntegrationTimeInMs(handle, 1, intetime);

	int irange = -1;
	if (range == "auto") {
		irange = -1;
	}
	else {
		irange = atoi(range.c_str());
	}
	ret +=GOMDX20_setRange(handle, curChannel, irange);

	int iwave = 0;
	if (wavelength == "365nm")
	{
		iwave = 1;
	}
	else if (wavelength == "375nm")
	{
		iwave = 2;
	}
	else if (wavelength == "385nm")
	{
		iwave = 3;
	}
	else if (wavelength == "395nm")
	{
		iwave = 4;
	}
	else if (wavelength == "405nm")
	{
		iwave = 5;
	}

	ret += GOMDX20_selectCalibrationEntry(handle, iwave);
	if (ret == 0) {
		root["code"] = 0;
	}
	else {
		root["code"] = -4002;
	}

	string msg = writer.write(root);
	theApp.onMessageCallback(msg, X20_MESSAGE);

	return;


	double distance = 0;

	ret = GOMDX20_getDistance(handle, &distance);

	DebugLog::writeLogF("distance :ret=%d %lf", ret, distance);


	bool isactive = true;
	//	GOMDX20_setSynchronization(handle, isactive);

	int p10ms = 200;
	//GOMDX20_setSynchronizationPeriod(handle, p10ms);


	//GOMDX20_saveConfig(handle, "D:\\MeasureBot\\proj\\MeasureBot\\x20bot.gdf");

	int mode, calindex;
	GOMDX20_getSelectedCalibrationEntry(handle, &mode, &calindex);

	DebugLog::writeLogF("SelectedCalibrationEntry:%d %d", mode, calindex);


	GOMDX20_isChannelActive(handle, curChannel, &isactive);


	int channel;
	GOMDX20_getFirstActiveChannel(handle, &channel);

	DebugLog::writeLogF("channel %d is active:%d 1th channel=%d", curChannel, isactive, channel);

	int unitNum = 0;

	GOMDX20_getChannelUnitNumber(handle, curChannel, &unitNum);
	DebugLog::writeLogF("unit is %d", unitNum);



}


int ThreadX20::initSdk(Json::Value& command)
{
	int l_rc = 0;

	l_rc = GOMDX20_setPassword("9ejkl3($");//pswd:zBp#771 // key:84319-166618
	if (l_rc != 0)
		DebugLog::writeLogF("x20 setPassword:ret=%d", l_rc);

	l_rc = GOMDX20_getHandle("X1_0", &handle);
	if (l_rc != 0)
		DebugLog::writeLogF("x20 gethandle:ret=%d", l_rc);

	Json::Value root;
	Json::FastWriter writer;
	root["cmd"] = "Connect";
	root["dev"] = "x20";
	if (handle > 0) 
	{
		{
			char fw[20], serial[20], deviceType[50], version[50];
			int rc, channel, nrOfCalibrations = 0;
			//infos
			GOMDX20_getFirmwareVersion(handle, fw);
			GOMDX20_getType(handle, deviceType);
			GOMDX20_getSerialNumber(handle, serial);
			GOMDX20_getDLLVersion(version);
			GOMDX20_getFirstActiveChannel(handle, &channel);
			//read max index (max index number of calibration entries, starts by 0)
			GOMDX20_getNumberOfCalibrations(handle, &nrOfCalibrations);

			DebugLog::writeLogF("x20 start :sdkver=%s fw=%s dectector=%s-%s  channel=%d nrOfCalibrations=%d\n",
				version,
				fw,
				deviceType,
				serial,
				channel,
				nrOfCalibrations
			);

			curChannel = channel;


			//read text from each index i
			char calibName[128] = { 0 };
			stringstream tmpstr("");
			for (int i = 0; i < nrOfCalibrations; i++)
			{
				rc = GOMDX20_getCalibrationEntryName(handle, i, calibName);
				if (rc == 0)
					wavelength_.push_back(string(calibName));
			}

			GOMDX20_selectCalibrationEntry(handle, 3);//0 -6

			int mode, calindex;
			GOMDX20_getSelectedCalibrationEntry(handle, &mode, &calindex);
			DebugLog::writeLogF("========cur mode=%d wavelength=%d %s", mode, calindex, wavelength_[calindex].c_str());
		}
		//GOMDX20_setChannelActive(handle,curChannel,true);

		connected_ = 1;
		root["code"] = 0;
		string msg = writer.write(root);
		theApp.onMessageCallback(msg, X20_MESSAGE);
		return 0;
	}
	else {
		root["code"] = -4001;
		string msg = writer.write(root);
		theApp.onMessageCallback(msg, X20_MESSAGE);

	}
	return -1;
}
int ThreadX20::releaseSdk() {
	GOMDX20_releaseHandle(handle);
	return 0;
}
int ThreadX20::handleShowDevinfo(Json::Value& command)
{
	char fw[20], serial[20], deviceType[50], version[50];
	int rc, nrOfCalibrations = 0;
	//infos
	GOMDX20_getFirmwareVersion(handle, fw);
	GOMDX20_getType(handle, deviceType);
	GOMDX20_getSerialNumber(handle, serial);
	GOMDX20_getDLLVersion(version);
	//read max index (max index number of calibration entries, starts by 0)
	GOMDX20_getNumberOfCalibrations(handle, &nrOfCalibrations);

	DebugLog::writeLogF("x20 info:sdkver=%s fw=%s sn=%s dtype=%s nrOfCalibrations=%d",
		version,
		fw,
		serial,
		deviceType,
		nrOfCalibrations
	);

	//read text from each index i
	char calibName[128] = { 0 };
	stringstream tmpstr("");
	for (int i = 0; i < nrOfCalibrations; i++)
	{
		rc = GOMDX20_getCalibrationEntryName(handle, i, calibName);
		if (rc == 0)
			tmpstr << i << ":"
			<< calibName << std::endl;
	}

	DebugLog::writeLogF("---%s---\n", tmpstr.str().c_str());


	return 0;
	stringstream tmpstr1("");
	char name[50];
	for (int a = 0; a < nrOfCalibrations; a++)
	{
		rc = GOMDX20_selectCalibrationEntry(handle, a);
		if (rc == 0)
		{
			//read all 4 possible channels per calibration index
			for (int i = 0; i < 4; i++)
			{
				rc = GOMDX20_getCalibrationEntryName(handle, i, name);
				if (rc == 0)
				{
					tmpstr1 << i
						<< a << " channel"
						<< i << ":"
						;
					tmpstr1 << name << std::endl;
				}
			}
		}
	}
	tmpstr1 << std::endl;
	DebugLog::writeLogF("1---%s---\n", tmpstr1.str().c_str());


	return 0;
}

