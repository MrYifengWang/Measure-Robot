#pragma once

#include "../ptslib/ptslib.h"
#include "MessageQueue.h"
#include <modbus.h>

class ThreadRtuSmacq;

typedef struct M2101_chn
{
	int chn;
	int type;
	char name[64];
}CHN_TEMP;
class SlaveRtu
{
public:
	SlaveRtu(int addr, void* pp);
	virtual ~SlaveRtu();

public:
	int doMeasure();
	int doCheck();
public:
	ThreadRtuSmacq* pprent_;
	int addrid_;
	int type_;
	int sn_;
	std::vector<CHN_TEMP> channels;
};

class ThreadRtuSmacq : public pts_thread {
public:
	ThreadRtuSmacq(MessageQueue*);
	~ThreadRtuSmacq(void);
public:
	static ThreadRtuSmacq* startThread(MessageQueue*);
protected:
	virtual bool onStart();
	virtual void run();

private:
	int setupSlave();
	int releaseSlave();
	int initSlave(char* com, int baudrate);

private:
	void handleMessage(Json::Value& command);
	void handleMeasureOnce(Json::Value& command);
	void handleMeasureConnect(Json::Value& command);
	void handleMeasureReConnect(Json::Value& command);
	void handleMeasureDisConnect(Json::Value& command);
	void handleMeasureCheck(Json::Value& command);




public:
	std::vector<SlaveRtu*> slaves_;
	MessageQueue* pQueue;
	modbus_t* pModbus;
	int connected_ = false;
	int automeasure_ = 0;
	int runCycle = 500;



};

