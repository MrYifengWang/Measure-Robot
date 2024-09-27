#pragma once

#include "../ptslib/ptslib.h"
#include "MessageQueue.h"
#include <modbus.h>


class ThreadRtuLx : public pts_thread {
public:
	ThreadRtuLx(MessageQueue*);
	~ThreadRtuLx(void);
public:
	static ThreadRtuLx* startThread(MessageQueue*);
protected:
	virtual bool onStart();
	virtual void run();
	int doMeasure();
	int checkStat();

private:
	void handleMessage(Json::Value& command);
	void handleTurn(Json::Value& command);
	void handleChannelSetChannel(Json::Value& command);
	void handleChannelInitChannel(Json::Value& command);
	void handleChannelSetRatio(Json::Value& command);

	void handleChannelTurn(Json::Value& command);

	void handleMeasureConnect(Json::Value& command);
	void handleMeasureReConnect(Json::Value& command);
	void handleMeasureDisConnect(Json::Value& command);
	void handleMeasureCheck(Json::Value& command);



private:
	int initSlave(char* com, int baudrate);
	int releaseSlave();

private:
	MessageQueue* pQueue;
	modbus_t* pModbus;
	//std::vector<CHN_TEMP1> channels;

	unsigned int channelBits = 0; // max 32 chn
	unsigned int channelOnBits = 0; // max 32 chn
	int connected_ = false;
	int runCycle = 1000;


};

