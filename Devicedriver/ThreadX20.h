#pragma once

#include "../ptslib/ptslib.h"
#include "MessageQueue.h"

class ThreadX20 : public pts_thread {
public:
	ThreadX20(MessageQueue*);
	~ThreadX20(void);
public:
	static ThreadX20* startThread(MessageQueue*);
protected:
	virtual bool onStart();
	virtual void run();

private:
	void handleMessage(Json::Value& command);
	void handleMeasureOnce(Json::Value& command);
	void handleMeasureAuto(Json::Value& command);
	void handleMeasureSet(Json::Value& command);
	int handleShowDevinfo(Json::Value& command);
	void handleMeasureStop(Json::Value& command);

	void handleMeasureConnect(Json::Value& command);
	void handleMeasureReConnect(Json::Value& command);
	void handleMeasureDisConnect(Json::Value& command);
	void handleMeasureCheck(Json::Value& command);


private:
	int initSdk(Json::Value& command);
	int releaseSdk();

private:
	MessageQueue* pQueue;
	int handle;
	int curChannel;
	int connected_ = false;
	std::vector<string> wavelength_;
	int runCyc = 500;

};

