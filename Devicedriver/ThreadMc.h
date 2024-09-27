#pragma once
#include "../ptslib/ptslib.h"
#include "MessageQueue.h"
//#pragma comment(lib,"./ptslib/Debug/ptslib.lib")
typedef struct d2_point {
	int axis = 0;
	int t = 0;
}D2point;
#define BasePulse	300;
class ThreadMc : public pts_thread {
public:
	ThreadMc(MessageQueue*);
	~ThreadMc(void);
public:
	static ThreadMc* startThread(MessageQueue*);
protected:
	virtual bool onStart();
	virtual void run();

private:
	void resetMc();
	void initMc(Json::Value& resp);
	void singleGohome(int iax,int dir);
	void singleJog(int iax, int dir);
	void goDriftXY();
	void goHome();
	void checkMcStat();
	void checkMcStatEx();
	void checkWheel(TAllSysStatusData& allstat, stringstream& info);
	void checkLimSns(TAllSysStatusData& allstat, stringstream& info);
	void checkPos(TAllSysStatusData& allstat, stringstream& info);
	void checkStat(TAllSysStatusData& allstat, stringstream& info);
	void checkVel(TAllSysStatusData& allstat, stringstream& info);


	void checkMcLimit();
	void handleMessage(Json::Value& command);
	void handleOpen(Json::Value& command);
	void handleStop(Json::Value& command);
	void handleMovPos(Json::Value& command);
	void handleTracksMov(Json::Value& command);
	void handleJogMov(Json::Value& command);
	void handleJogHome(Json::Value& command);
	void handleZero(Json::Value& command);

	void handleMeasureConnect(Json::Value& command);
	void handleMeasureReConnect(Json::Value& command);
	void handleMeasureDisConnect(Json::Value& command);
	void handleMeasureCheck(Json::Value& command);

	void respTrackStat(Json::Value& root);
	int getVelPulse(int mmpp);
	int movePoint(int vel, CPoint& st, CPoint& pt, CPoint& pulse, int group = -1, int group1 = 0);
	int movePoint1(int vel, CPoint& st, CPoint& pt, CPoint& pulse);
	int movePoint2(int vel);

	int nextPos(char ax, int dis,int vel, CPoint& pt, CPoint& pulse,int group=-1,int injob=0);
	void handleTracksTask1(Json::Value& command);
	void handleTracksTask2(Json::Value& command);
	void handleTracksTask3(Json::Value& command);
	void handleTaskPause(Json::Value& command);
	void handleTaskContinue(Json::Value& command);

	void responsePause(int code);

public:
	inline int rt(int src) {
		return (src);
	};
	inline void resetAddons() {
		for (int i = 0; i < 100; i++) {
			gapAddons[i] = 0.0;
		}
	}
	inline void getAddons(float adds[100]) {
		for (int i = 0; i < 100; i++) {
			gapAddons[i] = adds[i];
		}
	}

private:
	MultiCard g_MultiCard;
	pts_buffer m_wBuffer;
	MessageQueue* pQueue;

	CPoint pulse1; 
	CPoint point1;

	CPoint pulse2;
	CPoint point2;

	CPoint pulse3;
	CPoint point3;

	int poslimit1 =0;
	int neglimit1 =0;
	int poslimit2 = 0;
	int neglimit2 = 0;

	int curAxis = 1;

	float gapAddons[100];

	std::vector<D2point> tracks_;
	CPoint handPulse;
	int wheelX = 0;
	int wheelY = 0;
	int wheelRun = 0;
	int wheelSpeed = 1;
	int connected_ = false;
};

