#include "pch.h"
#include "framework.h"
#include "ThreadMc.h"
#include "MeasureBot.h"
#include "MeasureBotDlg.h"
#include <string>


const int PPMM = 300;
#define UnitPulse 300
ThreadMc::ThreadMc(MessageQueue* pq)
{
	pQueue = pq;
	handPulse.x = 0;
	handPulse.y = 0;
}

ThreadMc::~ThreadMc(void)
{

}
ThreadMc* ThreadMc::startThread(MessageQueue* pq) {
	ThreadMc* thread = new ThreadMc(pq);
	assert(thread);

	thread->setAutoDelete(true);
	thread->start();
	return thread;
}
bool ThreadMc::onStart() {

	{
		D2point p1, p2, p3, p4;
		p1.axis = 1; p1.t = 200;
		p2.axis = 2; p2.t = 200;
		p3.axis = 1; p3.t = 0;
		p4.axis = 2; p4.t = 0;

		tracks_.push_back(p1);
		tracks_.push_back(p2);
		tracks_.push_back(p3);
		tracks_.push_back(p4);
	}

	return true;
}

/*
* 脉冲 ---> 距离（mm）
测量(计算，根据丝杠螺纹参数)电机一圈走多少距离，然后设置一圈需要多少个脉冲 ，距离/脉冲数量 速度 脉冲/毫秒

需要计算轴的全程 （原点到限位）
需要计算有效测试行程，（光表尺寸）
由伺服驱动器的电子齿轮设置决定的，例如，编码器是10000pps,电子齿轮比设为1，则电机一圈需要10000PPS。若设为2，则电机一圈需5000PPS。简单说就是，10000=（电子齿轮比）*（PLC发的脉冲数）。
而伺服电机上的编码器精度是固定的，比如伺服电机转一圈，编码器10000个脉冲，可以改变驱动器上的电子齿轮比改变控制精度。

伺服说明书

导轨参数

导程=转动一圈的移动量

导程=丝杆的螺距*螺纹的条数
*/

void ThreadMc::handleMeasureConnect(Json::Value& command) {}
void ThreadMc::handleMeasureReConnect(Json::Value& command) {}
void ThreadMc::handleMeasureDisConnect(Json::Value& command) {}
void ThreadMc::handleMeasureCheck(Json::Value& command) {}
void ThreadMc::run() {

	while (1)
	{
		Message* pmsg = pQueue->trypop(1000);
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
		else {
		}
		checkMcStatEx();
	}
}
void ThreadMc::singleGohome(int iAxisNum, int dir) {

	TAxisHomePrm homePrm;
	homePrm.nHomeMode = 1;
	homePrm.nHomeDir = dir;
	homePrm.dHomeAcc = 0.1;
	if (dir == 1) {
		homePrm.lOffset = 5 * PPMM;
	}
	else {
		homePrm.lOffset = -5 * PPMM;
	}

	homePrm.dHomeRapidVel = 10;
	homePrm.dHomeLocatVel = 10;
//	homePrm.lOffset = 0;

	/*TAxisHomePrm homePrmout;
	g_MultiCard.MC_HomeGetPrm(iAxisNum, &homePrmout);*/

	DebugLog::writeLogF("===gohome start:%d dir=%d", iAxisNum, dir);

	/*
	short		nHomeMode;					//回零方式：0--无 1--HOME回原点	2--HOME加Index回原点3----Z脉冲
	short		nHomeDir;					//回零方向，1-正向回零，0-负向回零
	long        lOffset;                    //回零偏移，回到零位后再走一个Offset作为零位

	double		dHomeRapidVel;			    //回零快移速度，单位：Pluse/ms
	double		dHomeLocatVel;			    //回零定位速度，单位：Pluse/ms
	double		dHomeIndexVel;			    //回零寻找INDEX速度，单位：Pluse/ms
	double      dHomeAcc;                   //回零使用的加速度

	long ulHomeIndexDis;           //找Index最大距离
	long ulHomeBackDis;            //回零时，第一次碰到零位后的回退距离
	unsigned short nDelayTimeBeforeZero;    //位置清零前，延时时间，单位ms
	unsigned long ulHomeMaxDis;
	DebugLog::writeLogF("go home params:mod=%d dir=%d %ld -- %lf %lf %lf %lf --%ld %ld %u %lu",
		homePrmout.nHomeMode,
		homePrmout.nHomeDir,
		homePrmout.lOffset,
		homePrmout.dHomeRapidVel,
		homePrmout.dHomeLocatVel,
		homePrmout.dHomeIndexVel,
		homePrmout.dHomeAcc,
		homePrmout.ulHomeIndexDis,
		homePrmout.ulHomeBackDis,
		homePrmout.nDelayTimeBeforeZero, homePrmout.ulHomeMaxDis);
		*/

	g_MultiCard.MC_HomeSetPrm(iAxisNum, &homePrm);
	g_MultiCard.MC_HomeStart(iAxisNum);
}
void ThreadMc::singleJog(int iAxisNum, int dir) {

	int iRes = 0;
	TJogPrm m_JogPrm;

	m_JogPrm.dAcc =  0.1;
	m_JogPrm.dDec =  0.1;
	m_JogPrm.dSmooth = 0;

	iRes = g_MultiCard.MC_PrfJog(iAxisNum);
	iRes += g_MultiCard.MC_SetJogPrm(iAxisNum, &m_JogPrm);
	iRes += g_MultiCard.MC_SetVel(iAxisNum, dir * 10.0);
	iRes += g_MultiCard.MC_Update(0X0001 << (iAxisNum - 1));
	pts_time::wait(2000);
	g_MultiCard.MC_Stop(0X0001 << (iAxisNum - 1), 0);
	g_MultiCard.MC_HomeStop(iAxisNum);

}

void ThreadMc::goDriftXY() {
	CMeasureBotDlg* pdlg = (CMeasureBotDlg*)theApp.m_pMainWnd;

	int xDf = ( pdlg->m_page_track.getSrart('x'))*(-1);//-250;
	int yDf = (pdlg->m_page_track.getSrart('y')) * (1);//450
	int vel = 60;
	g_MultiCard.MC_ZeroPos(1);
	g_MultiCard.MC_ZeroPos(2);

	CPoint pulse(0, 0);
	CPoint st(0, rt(0));
	CPoint pt(xDf, yDf);

	int count = 0;
	int iRes = 0;
	int isContinue = 1;
	int realmov = 0;

	
		for (int i = 1; i <= 2; i++) {
			int iAxisNum = i;
			int m_lTargetPosPulse = 0;
			if (i == 1) {
				pulse.x = pt.x * PPMM;
				m_lTargetPosPulse = pulse.x;
				realmov = 1;
			}
			else if (i == 2) {
				pulse.y = pt.y * PPMM;
				m_lTargetPosPulse = pulse.y;
				realmov = 1;
			}


			int iRes = 0;

			TTrapPrm TrapPrm;
			TrapPrm.acc = 0.1; //加速度
			TrapPrm.dec = 0.1; //减速度
			TrapPrm.smoothTime = 50; //平滑时间
			TrapPrm.velStart = 0; //启动速度

			iRes = g_MultiCard.MC_PrfTrap(iAxisNum);
			iRes += g_MultiCard.MC_SetTrapPrm(iAxisNum, &TrapPrm);
			iRes += g_MultiCard.MC_SetPos(iAxisNum, m_lTargetPosPulse);
			iRes = g_MultiCard.MC_SetVel(iAxisNum, double((long long)vel  * PPMM / 1000.0));

			iRes += g_MultiCard.MC_Update(0X0001 << (iAxisNum - 1));

		}
		
		while (1) {
			pts_time::wait(500);
			//count++;
			TAllSysStatusData m_AllSysStatusDataTemp;
			iRes = g_MultiCard.MC_GetAllSysStatus(&m_AllSysStatusDataTemp);

			int xpos = int(m_AllSysStatusDataTemp.dAxisPrfPos[0] / 300);
			int ypos = int(m_AllSysStatusDataTemp.dAxisPrfPos[1] / 300);

			if (ypos == (pulse.y / 300) && xpos == pulse.x / 300) {
				break;
			}
		}
}


void ThreadMc::goHome()
{
	//g_MultiCard.MC_ClrSts(1);
	//g_MultiCard.MC_ClrSts(2);
	//g_MultiCard.MC_HomeStop(1);
	//g_MultiCard.MC_HomeStop(2);
	g_MultiCard.MC_Reset();
	Json::Value response;
	initMc(response);

	int dir[3] = {0,1,0};
	int success = 0;
	
	for (int i = 1; i <= 2; i++) {
		int iAxisNum = i;

		unsigned short goHomeStat = 0;
		g_MultiCard.MC_HomeGetSts(iAxisNum, &goHomeStat);
		if (goHomeStat == 2) {
			continue;
		}
		singleGohome(iAxisNum, dir[i]);
	}

	while (1) {
		pts_time::wait(500);
		unsigned short goHomeStat1 = 0;
		unsigned short goHomeStat2 = 0;
		g_MultiCard.MC_HomeGetSts(1, &goHomeStat1);
		g_MultiCard.MC_HomeGetSts(2, &goHomeStat2);
		if (goHomeStat1 == 2) {
			g_MultiCard.MC_HomeStop(1);

		}
		if (goHomeStat2 == 2) {
			g_MultiCard.MC_HomeStop(2);

		}
		if (goHomeStat1 == 2 && goHomeStat2 == 2) {
			success = 1;
			DebugLog::writeLogF("gohome success!");
			return;
		}

		unsigned long int lValue = 0;
		g_MultiCard.MC_GetExtDiValue(0, &lValue, 1);

		if (lValue == 0) return;

		poslimit1 = 0;
		neglimit1 = 0;
		poslimit2 = 0;
		neglimit2 = 0;
		if (!(lValue & 1)) {
			neglimit1 = 1;
		}
		if (!(lValue & 1 << 1)) {
			poslimit1 = 1;
		}
		if (!(lValue & 1 << 2)) {
			poslimit2 = 1;
		}
		if (!(lValue & 1 << 3)) {
			neglimit2 = 1;
		}

		if (dir[1] == 1 && poslimit1 == 1) {
			dir[1] = 0;
			singleJog(1,-1);
			singleGohome(1, dir[1]);

		}
		if (dir[1] == 0 && neglimit1 == 1) {
			dir[1] = 1;
			singleJog(1, 1);
			singleGohome(1, dir[1]);

		}
		if (dir[2] == 0 && neglimit2 == 1) {
			dir[2] = 1;
			singleJog(2, 1);
			singleGohome(2, dir[2]);

		}
		if (dir[2] == 1 && poslimit2 == 1) {
			dir[2] = 0;
			singleJog(2, -1);
			singleGohome(2, dir[2]);
		}

	}
}
void ThreadMc::checkMcLimit()
{
	if (!connected_)
		return;
	int xZero = 0;
	int yZero = 0;
	int iAxisNum = curAxis;
	int iRes = 0;
	unsigned long lValue = 0;
	unsigned short goHomeStat = 0;
	g_MultiCard.MC_HomeGetSts(iAxisNum, &goHomeStat);
	g_MultiCard.MC_GetDiRaw(3, (long*)&lValue);
	if (!(lValue & 1)) {
		xZero = 1;
	}
	if (!(lValue & 1 << 1)) {
		yZero = 1;
	}

	//通用输入IO信号显示
	iRes = g_MultiCard.MC_GetExtDiValue(0, &lValue, 1);

	if (lValue == 0) return;

	poslimit1 = 0;
	neglimit1 = 0;
	poslimit2 = 0;
	neglimit2 = 0;
	if (!(lValue & 1)) {
		poslimit1 = 1;
	}
	if (!(lValue & 1 << 1)) {
		neglimit1 = 1;
	}
	if (!(lValue & 1 << 2)) {
		poslimit2 = 1;
	}
	if (!(lValue & 1 << 3)) {
		neglimit2 = 1;
	}

	//DebugLog::writeLogF("-------axis stat:%d  limit:xp=%d xn=%d yp=%d yn=%d Zero x=%d y=%d", goHomeStat, poslimit1, neglimit1, poslimit2, neglimit2, xZero, yZero);


	if (poslimit1 == 1 || neglimit1 == 1) {
		g_MultiCard.MC_Stop(0X1, 0X1);
	}
	if (poslimit2 == 1 || neglimit2 == 1) {
		g_MultiCard.MC_Stop(0X2, 0X2);
	}
}

void ThreadMc::checkMcStatEx() {
	if (!connected_)
		return;
	int iRes = 0;
	unsigned long lValue = 0;

	int iAxisNum = 1;
	int iCardIndex = 0;

	TAllSysStatusData m_AllSysStatusDataTemp;
	stringstream info("");
	iRes = g_MultiCard.MC_GetAllSysStatus(&m_AllSysStatusDataTemp);
	//检测手轮使能
	checkWheel(m_AllSysStatusDataTemp, info);
	checkLimSns(m_AllSysStatusDataTemp, info);
//	checkPos(m_AllSysStatusDataTemp, info);
	checkStat(m_AllSysStatusDataTemp, info);
	//checkVel(m_AllSysStatusDataTemp, info);

//	DebugLog::writeLog((char*)info.str().c_str());

	return;


	for (int iax = 1; iax <= 2; iax++) {
		double dCrdVel;
		g_MultiCard.MC_GetCrdVel(iax, &dCrdVel);

		double dPos[8];
		iRes = g_MultiCard.MC_GetCrdPos(iax, dPos);

		short nCrdStatus = 0;
		iRes = g_MultiCard.MC_CrdStatus(iax, &nCrdStatus, NULL);
	}
}
void ThreadMc::checkWheel(TAllSysStatusData& alldata, stringstream& info)
{
	//X
	if (alldata.lGpiRaw[0] & 1 << (12))
	{
		if (wheelX == 0) {
			wheelX = 1;
			g_MultiCard.MC_EndHandwheel(1);
			g_MultiCard.MC_EndHandwheel(2);
			g_MultiCard.MC_StartHandwheel(1, 9, 1, wheelSpeed, 0, 0.1, 0.1, 50, 0);
			info << "x wheel set on\t";
		}
	}
	else
	{
		if (wheelX == 1) {
			wheelX = 0;
			g_MultiCard.MC_EndHandwheel(1);
			info << "x wheel set off\t";
		}
	}

	//Y
	if (alldata.lGpiRaw[0] & 1 << (13))
	{
		if (wheelY == 0) {
			wheelY = 1;
			g_MultiCard.MC_EndHandwheel(1);
			g_MultiCard.MC_EndHandwheel(2);
			g_MultiCard.MC_StartHandwheel(2, 9, 1, wheelSpeed, 0, 0.1, 0.1, 50, 0);
			info << "y wheel set on\t";
		}
	}
	else
	{
		if (wheelY == 1) {
			wheelY = 0;
			g_MultiCard.MC_EndHandwheel(2);
			info << "y wheel set off\t";
		}
	}
	//X1
	if (alldata.lGpiRaw[0] & 1 << (14)) {

		if (wheelSpeed != 1) {
			wheelSpeed = 1;
			if (wheelY == 1) {
				g_MultiCard.MC_EndHandwheel(2);
				g_MultiCard.MC_StartHandwheel(2, 9, 1, wheelSpeed, 0, 0.1, 0.1, 50, 0);
			}
			else if (wheelX == 1) {
				g_MultiCard.MC_EndHandwheel(1);
				g_MultiCard.MC_StartHandwheel(1, 9, 1, wheelSpeed, 0, 0.1, 0.1, 50, 0);
			}
		}

	}
	//X1
	if (alldata.lGpiRaw[0] & 1 << (15)) {

		if (wheelSpeed != 10) {
			wheelSpeed = 10;
			if (wheelY == 1) {
				g_MultiCard.MC_EndHandwheel(2);
				g_MultiCard.MC_StartHandwheel(2, 9, 1, wheelSpeed, 0, 0.1, 0.1, 50, 0);
			}
			else if (wheelX == 1) {
				g_MultiCard.MC_EndHandwheel(1);
				g_MultiCard.MC_StartHandwheel(1, 9, 1, wheelSpeed, 0, 0.1, 0.1, 50, 0);
			}
		}

	}
	if ((0 == (alldata.lGpiRaw[0] & 1 << (15))) && (0 == (alldata.lGpiRaw[0] & 1 << (14))))
	{
		if (wheelSpeed != 100) {
			wheelSpeed = 100;
			if (wheelY == 1) {
				g_MultiCard.MC_EndHandwheel(2);
				g_MultiCard.MC_StartHandwheel(2, 9, 1, wheelSpeed, 0, 0.1, 0.1, 50, 0);
			}
			else if (wheelX == 1) {
				g_MultiCard.MC_EndHandwheel(1);
				g_MultiCard.MC_StartHandwheel(1, 9, 1, wheelSpeed, 0, 0.1, 0.1, 50, 0);
			}
		}
	}
	info << "\n";

}
void ThreadMc::checkVel(TAllSysStatusData& allstat, stringstream& info) {
	info << "vel:x " << allstat.dCrdVel[0] << " y " << allstat.dCrdVel[1] << " pos:x" << allstat.dCrdPos[0] << " y " << allstat.dCrdPos[1] << "\n";
}
void ThreadMc::checkPos(TAllSysStatusData& allstat, stringstream& info) {
	for (int iax = 1; iax <= 2; iax++) {
		info << "ax " << iax << " pos: " << allstat.dAxisEncPos[iax - 1] << ", " << allstat.dAxisPrfPos[iax - 1] << "\t";
	}
	info << "\n";
}
void ThreadMc::checkStat(TAllSysStatusData& allstat, stringstream& info) {
	for (int iax = 1; iax <= 2; iax++) {
		info << "ax " << iax << " stat:" << allstat.lAxisStatus << "\t";
	}
	info << "\n";
}
void ThreadMc::checkLimSns(TAllSysStatusData& allstat, stringstream& info) {

	info << "limsns:home=" << allstat.lHomeRaw; 
	info << " pos=" << allstat.lLimitPosRaw << "neg=" << allstat.lLimitNegRaw << "\t";
	//DebugLog::writeLogF("---limit %04X %04X %04X",allstat.lHomeRaw, allstat.lLimitPosRaw, allstat.lLimitNegRaw);
}

void ThreadMc::checkMcStat() {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (!connected_)
		return;
	int iRes = 0;
	unsigned long lValue = 0;

	int iAxisNum = 1;
	int iCardIndex = 0;

	TAllSysStatusData m_AllSysStatusDataTemp;
	iRes = g_MultiCard.MC_GetAllSysStatus(&m_AllSysStatusDataTemp);
	//检测手轮使能

	double dCrdVel;
	g_MultiCard.MC_GetCrdVel(1, &dCrdVel);

	//读取板卡状态数据。

	double dPos[8];
	iRes = g_MultiCard.MC_GetCrdPos(1, dPos);

	short nCrdStatus = 0;
	iRes = g_MultiCard.MC_CrdStatus(1, &nCrdStatus, NULL);

	long plimit, nlimit;
	g_MultiCard.MC_GetSoftLimit(iAxisNum, &plimit, &nlimit);

	/*DebugLog::writeLogF("all status: prfpos=%lf encpos=%lf axisstat=%d %d %d %d lGpiRaw=%02X lLimitNegRaw=%02X lLimitPosRaw=%02X lMPG=%02X  xpos=%lf ypos=%lf nCrdStatus=%02X softLimit = %ld %ld\n",
		m_AllSysStatusDataTemp.dAxisPrfPos[iAxisNum - 1],
		m_AllSysStatusDataTemp.dAxisPrfPos[iAxisNum - 1],
		m_AllSysStatusDataTemp.lAxisStatus[iAxisNum - 1] & AXIS_STATUS_SV_ALARM,
		m_AllSysStatusDataTemp.lAxisStatus[iAxisNum - 1] & AXIS_STATUS_RUNNING,
		m_AllSysStatusDataTemp.lAxisStatus[iAxisNum - 1] & AXIS_STATUS_POS_HARD_LIMIT,
		m_AllSysStatusDataTemp.lAxisStatus[iAxisNum - 1] & AXIS_STATUS_NEG_HARD_LIMIT,
		m_AllSysStatusDataTemp.lGpiRaw[0],
		m_AllSysStatusDataTemp.lLimitNegRaw,
		m_AllSysStatusDataTemp.lLimitPosRaw,
		m_AllSysStatusDataTemp.lMPG, dPos[0], dPos[1], nCrdStatus, plimit, nlimit

	);*/



	//负硬限位IO输入信号显示
		//iRes = g_MultiCard.MC_GetDiRaw(MC_LIMIT_NEGATIVE,(long*)&lValue);

	//正限位IO输入信号显示
		//iRes = g_MultiCard.MC_GetDiRaw(MC_LIMIT_POSITIVE,(long*)&lValue);

	//手轮IO输入状态显示lMPG
		//X

	//坐标系运行状态显示 nCrdStatus
}


void ThreadMc::handleMessage(Json::Value& command)
{
	string cmd = command["cmd"].asString();

	if (cmd != "Connect") {
		if (!connected_)
			return;
	}

	if (cmd == "Jog") {
		handleJogMov(command);
	}
	else
		if (cmd == "Joghome") {
			handleJogHome(command);
		}
		else if (cmd == "Pos") {
			handleMovPos(command);
			//handleTracksMov(command);
		}
		else if (cmd == "TrackTask") {
			int model = command["model"].asInt();
			if (model == 1)
				handleTracksTask1(command);
			else if (model == 2)
				handleTracksTask2(command);
			else if (model == 3)
				handleTracksTask3(command);
		}
		else if (cmd == "Zero") {
			handleZero(command);
		}
		else if (cmd == "Stop") {
			handleStop(command);
		}
		else if (cmd == "Connect") {
			handleOpen(command);
		}
		else if (cmd == "Close") {
			g_MultiCard.MC_Close();
		}
		else if (cmd == "Home") {
			goHome();
		//	goDriftXY();
		}
		else if (cmd == "Pause") {
			handleTaskPause(command);
		}
		else if (cmd == "Continue") {
			handleTaskContinue(command);
		}
		else if (cmd == "Exit") {
			handleJogMov(command);
		}

}

int ThreadMc::getVelPulse(int mmpp) {
	int fixpart = mmpp * PPMM / 1000;
	int modpart = mmpp * PPMM % 1000;
	int objpulse = fixpart + modpart >= 500 ? 1 : 0;

	objpulse = objpulse == 0 ? 1 : objpulse;
	objpulse = objpulse >= 20 ? 20 : objpulse;

	return objpulse;
}
int ThreadMc::movePoint(int vel, CPoint& st, CPoint& pt, CPoint& pulse, int group,int group1) {

	int count = 0;
	int iRes = 0;
	int isContinue = 1;
	int realmov = 0;

	while (isContinue) {
		for (int i = 1; i <= 2; i++) {
			int iAxisNum = i;
			int m_lTargetPosPulse = 0;
			if (i == 1) {
				if (pulse.x == (pt.x - st.x) * PPMM)
					continue;
				if (group == 0) {
					pulse.x = (pt.x - st.x + gapAddons[group1]) * PPMM;

				}
				else {
					pulse.x = (pt.x - st.x) * PPMM;

				}
				m_lTargetPosPulse = pulse.x;
				realmov = 1;
			}
			else if (i == 2) {
				if (pulse.y == -1 * ((pt.y - st.y) * PPMM))
					continue;
				pulse.y = -1*((pt.y - st.y) * PPMM);
				m_lTargetPosPulse = pulse.y;
				realmov = 1;
			}

			DebugLog::writeLogF("track 3 point=== %d %d  ===%d %d", pt.x, pt.y,pulse.x/300,pulse.y/300);
			//continue;


			int iRes = 0;
			//g_MultiCard.MC_AxisOn(iAxisNum);

			TTrapPrm TrapPrm;
			TrapPrm.acc = 0.1; //加速度
			TrapPrm.dec = 0.1; //减速度
			TrapPrm.smoothTime = 50; //平滑时间
			TrapPrm.velStart = 0; //启动速度

			iRes = g_MultiCard.MC_PrfTrap(iAxisNum);
			iRes += g_MultiCard.MC_SetTrapPrm(iAxisNum, &TrapPrm);
			iRes += g_MultiCard.MC_SetPos(iAxisNum, m_lTargetPosPulse);
			iRes = g_MultiCard.MC_SetVel(iAxisNum, double((long long)vel*2 * PPMM / 1000.0));

			iRes += g_MultiCard.MC_Update(0X0001 << (iAxisNum - 1));

		}
		if (realmov == 0)
			break;
		while (1) {
			pts_time::wait(500);
			//count++;
			TAllSysStatusData m_AllSysStatusDataTemp;
			iRes = g_MultiCard.MC_GetAllSysStatus(&m_AllSysStatusDataTemp);
			double Vel2[2] = {0.0};
			g_MultiCard.MC_GetPrfVel(1, Vel2, 2);

			int xpos = int(m_AllSysStatusDataTemp.dAxisPrfPos[0] / 300);
			int ypos = int(m_AllSysStatusDataTemp.dAxisPrfPos[1] / 300);

			if (ypos == (pulse.y / 300) && xpos == pulse.x / 300) {

				Json::Value root;
				Json::FastWriter writer;
				root["cmd"] = "Track";
				root["sub"] = "point";
				root["dev"] = "mc";
				root["code"] = 0;
				root["xvel"] = Vel2[0];
				root["yvel"] = Vel2[1];
				root["xpos"] = pt.x;
				root["ypos"] = pt.y;
				root["xpulse"] = pulse.x/300;
				root["ypulse"] = pulse.y/300;
				string msg = writer.write(root);
				theApp.onMessage2SettingMC(msg, MC_MESSAGE);
				isContinue = 0;
				break;
			}
			//if (count > 30)break;

			int idx = pQueue->peek("track_stop");
			if (idx >= 0) {
				isContinue = 0;
				g_MultiCard.MC_Stop(0XFFFFFFFF, 0XFFFFFFFF);
				DebugLog::writeLogF("Track mov: received stop!");
				return -1;
			}
			int idx1 = pQueue->prepop("track_pause");
			if (idx1 >= 0) {
				g_MultiCard.MC_Stop(0XFFFFFFFF, 0XFFFFFFFF);
				DebugLog::writeLogF("Track mov: received pause!");
				while (1) {
					pts_time::wait(200);
					int idx2 = pQueue->prepop("track_continue");
					if (idx2 >= 0) {
						isContinue = 1;
						break;
					}

					int idx3 = pQueue->prepop("track_stop");
					if (idx3 >= 0) {
						return -1;
					}
				}
				if (isContinue == 1) {
					break;
				}
			}
		}
	}
}

int ThreadMc::movePoint1(int vel, CPoint& st, CPoint& pt, CPoint& pulse) {

	int count = 0;
	int iRes = 0;
	int isContinue = 1;
	int realmov = 0;

	while (isContinue) {
		for (int i = 1; i <= 2; i++) {
			int iAxisNum = i;
			int m_lTargetPosPulse = 0;
			if (i == 1) {
				if (pt.x == 0)
					continue;
				if (pulse.x == (pt.x - st.x) * PPMM)
					continue;
				pulse.x = (pt.x - st.x) * PPMM;
				m_lTargetPosPulse = pulse.x;
				realmov = 1;
			}
			else if (i == 2) {
				if (pt.y == 0)
					continue;
				if (pulse.y == 1 * ((pt.y - st.y) * PPMM))
					continue;
				pulse.y = 1 * ((pt.y - st.y) * PPMM);
				m_lTargetPosPulse = pulse.y;
				realmov = 1;
			}

			DebugLog::writeLogF("track 3 point=== %d %d  ===%d %d", pt.x, pt.y, pulse.x / 300, pulse.y / 300);
			//continue;


			int iRes = 0;
			//g_MultiCard.MC_AxisOn(iAxisNum);

			TTrapPrm TrapPrm;
			TrapPrm.acc = 0.1; //加速度
			TrapPrm.dec = 0.1; //减速度
			TrapPrm.smoothTime = 50; //平滑时间
			TrapPrm.velStart = 0; //启动速度

			iRes = g_MultiCard.MC_PrfTrap(iAxisNum);
			iRes += g_MultiCard.MC_SetTrapPrm(iAxisNum, &TrapPrm);
			iRes += g_MultiCard.MC_SetPos(iAxisNum, m_lTargetPosPulse);
			iRes = g_MultiCard.MC_SetVel(iAxisNum, double((long long)vel * 2 * PPMM / 1000.0));

			iRes += g_MultiCard.MC_Update(0X0001 << (iAxisNum - 1));

		}
		if (realmov == 0)
			break;
		while (1) {
			pts_time::wait(500);
			//count++;
			TAllSysStatusData m_AllSysStatusDataTemp;
			iRes = g_MultiCard.MC_GetAllSysStatus(&m_AllSysStatusDataTemp);
			double Vel2[2] = { 0.0 };
			g_MultiCard.MC_GetPrfVel(1, Vel2, 2);

			int xpos = int(m_AllSysStatusDataTemp.dAxisPrfPos[0] / 300);
			int ypos = int(m_AllSysStatusDataTemp.dAxisPrfPos[1] / 300);
			{
				Json::Value root;
				Json::FastWriter writer;
				root["cmd"] = "Pos";
				root["sub"] = "point";
				root["dev"] = "mc";
				root["code"] = 0;
				root["xvel"] = Vel2[0];
				root["yvel"] = Vel2[1];
				root["xpos"] = pt.x;
				root["ypos"] = pt.y;
				root["xpulse"] = xpos;
				root["ypulse"] = ypos;
				string msg = writer.write(root);
				theApp.onMessage2SettingMC(msg, MC_MESSAGE);
			}
			if ((ypos == (pulse.y / 300)&&  pt.x == 0)||( xpos == pulse.x / 300)&&pt.y==0) {

				
				isContinue = 0;
				break;
			}
			//if (count > 30)break;

			int idx = pQueue->peek("track_stop");
			if (idx >= 0) {
				isContinue = 0;
				g_MultiCard.MC_Stop(0XFFFFFFFF, 0XFFFFFFFF);
				DebugLog::writeLogF("Track mov: received stop!");
				return -1;
			}
			int idx1 = pQueue->prepop("track_pause");
			if (idx1 >= 0) {
				g_MultiCard.MC_Stop(0XFFFFFFFF, 0XFFFFFFFF);
				DebugLog::writeLogF("Track mov: received pause!");
				while (1) {
					pts_time::wait(200);
					int idx2 = pQueue->prepop("track_continue");
					if (idx2 >= 0) {
						isContinue = 1;
						break;
					}

					int idx3 = pQueue->prepop("track_stop");
					if (idx3 >= 0) {
						return -1;
					}
				}
				if (isContinue == 1) {
					break;
				}
			}
		}
	}
}

int ThreadMc::movePoint2(int vel) {

	int count = 0;
	int iRes = 0;
	int isContinue = 1;
	int realmov = 0;


	for (int i = 1; i <= 2; i++) {
		int iAxisNum = i;
		int iRes = 0;

		TTrapPrm TrapPrm;
		TrapPrm.acc = 0.1; //加速度
		TrapPrm.dec = 0.1; //减速度
		TrapPrm.smoothTime = 50; //平滑时间
		TrapPrm.velStart = 0; //启动速度

		iRes = g_MultiCard.MC_PrfTrap(iAxisNum);
		iRes += g_MultiCard.MC_SetTrapPrm(iAxisNum, &TrapPrm);
		iRes += g_MultiCard.MC_SetPos(iAxisNum, 0);
		iRes = g_MultiCard.MC_SetVel(iAxisNum, double((long long)vel * 3 * PPMM / 1000.0));

		iRes += g_MultiCard.MC_Update(0X0001 << (iAxisNum - 1));

	}

	return 0;
	
}

void ThreadMc::responsePause(int code) {
	Json::Value root;
	root["cmd"] = "Track";
	root["sub"] = "step";
	root["dev"] = "mc";
	root["tid"] = 0;
	root["code"] = code; //2
	Json::FastWriter writer;
	string msg = writer.write(root);
	theApp.onMessage2SettingMC(msg, MC_MESSAGE);
}

int ThreadMc::nextPos(char ax, int dis_1, int vel, CPoint& pt, CPoint& pulse, int group, int injob) {

	int dis = dis_1;
	int iAxisNum = 0;
	int m_lTargetPosPulse = 0;
	if (ax == 'x') {
		pt.x += dis;
		pulse.x +=( dis+ gapAddons[group]) * PPMM;
		//	pulse.x += dis * PPMM;
		m_lTargetPosPulse = pulse.x;
		iAxisNum = 1;
	}
	else if (ax == 'y') {
		pt.y += dis;
		pulse.y -= dis * PPMM;
		m_lTargetPosPulse = pulse.y;
		iAxisNum = 2;
	}

	int isContinue = 1;

	while (isContinue) {
		int iRes = 0;
		//g_MultiCard.MC_AxisOn(iAxisNum);

		TTrapPrm TrapPrm;
		TrapPrm.acc = 0.1; //加速度
		TrapPrm.dec = 0.1; //减速度
		TrapPrm.smoothTime = 10; //平滑时间
		TrapPrm.velStart = 0; //启动速度

		iRes = g_MultiCard.MC_PrfTrap(iAxisNum);
		iRes += g_MultiCard.MC_SetTrapPrm(iAxisNum, &TrapPrm);
		iRes += g_MultiCard.MC_SetPos(iAxisNum, m_lTargetPosPulse);
		iRes = g_MultiCard.MC_SetVel(iAxisNum, double((long long)vel * PPMM / 1000.0));

		iRes += g_MultiCard.MC_Update(0X0001 << (iAxisNum - 1));

		//g_MultiCard.MC_AxisOff(iAxisNum);

		int count = 0;
		isContinue = 0;
		while (1) {
			if (injob == 1) {
				pts_time::wait(100);
			}
			else {
				pts_time::wait(200);

			}
			//count++;
			TAllSysStatusData m_AllSysStatusDataTemp;
			iRes = g_MultiCard.MC_GetAllSysStatus(&m_AllSysStatusDataTemp);
			double Vel2[2] = { 0.0 };
			g_MultiCard.MC_GetPrfVel(1, Vel2, 2);
			/*double dCrdVel;
			g_MultiCard.MC_GetCrdVel(1, &dCrdVel);
			double dPos[8];
			iRes = g_MultiCard.MC_GetCrdPos(iax, dPos);
			*/

			int dstpos = int(m_AllSysStatusDataTemp.dAxisPrfPos[iAxisNum - 1] / 300);

			//DebugLog::writeLogF("track mov: axis=%d  prfpos=%f encpos=%f  ret=%d   ---- pos=%d\n", iAxisNum, m_AllSysStatusDataTemp.dAxisPrfPos[iAxisNum - 1] / 3000,
			//	m_AllSysStatusDataTemp.dAxisEncPos[iAxisNum - 1] / 3000, iRes, dstpos);
			{
				Json::Value root;
				Json::FastWriter writer;
				root["cmd"] = "Track";
				root["sub"] = "pos";
				root["dev"] = "mc";
				root["code"] = 0;
				root["xvel"] = Vel2[0];
				root["yvel"] = Vel2[1];
				root["xpos"] = pt.x;
				root["ypos"] = pt.y;
				root["dir"] = dis;
				root["tm"] = (double)pts_time::currentms();
				string tmpax;
				tmpax = ax;
				root["axis"] = tmpax;
				root["gidx"] = group;

				if (ax == 'y') {
					root["xpulse"] = pulse.x/300;
					root["ypulse"] = dstpos;
				}
				else {
					root["xpulse"] = dstpos;
					root["ypulse"] = pulse.y/300;
				}
				string msg = writer.write(root);
				theApp.onMessage2SettingMC(msg, MC_MESSAGE);
			}
			if (ax == 'y' && dstpos == pulse.y / 300) break;
			if (ax == 'x' && dstpos == pulse.x / 300) break;
			//if (count > 30)break;
			int idx = pQueue->peek("track_stop");
			if (idx >= 0) {
				g_MultiCard.MC_Stop(0XFFFFFFFF, 0XFFFFFFFF);
				DebugLog::writeLogF("Track mov: received stop!");
				return -1;
			}
			int idx1 = pQueue->prepop("track_pause");
			if (idx1 >= 0) {
				g_MultiCard.MC_Stop(0XFFFFFFFF, 0XFFFFFFFF);
				DebugLog::writeLogF("Track mov: received pause!");
				//responsePause(2);
				while (1) {
					pts_time::wait(200);
					int idx2 = pQueue->prepop("track_continue");
					if (idx2 >= 0) {
						isContinue = 1;
						//responsePause(1);
						break;
					}

					int idx3 = pQueue->prepop("track_stop");
					if (idx3 >= 0) {
						return -1;
					}
				}
				if (isContinue == 1) {
					break;
				}
			}
		}
	}
	return 0;

}
void ThreadMc::respTrackStat(Json::Value& root) {

}

void ThreadMc::handleTracksTask1(Json::Value& command) {

	//--1
	Json::Value root;
	root["cmd"] = "Track";
	root["sub"] = "step";
	root["tid"] = 1;
	root["dev"] = "mc";
	root["code"] = 0;


	do {
		int repeat = command["repeat"].asInt();
		int vel = command["vel"].asInt();
		if (vel > 100) {
			root["code"] = -1;
			break;
		}
		CMeasureBotDlg* pdlg = (CMeasureBotDlg*)theApp.m_pMainWnd;

		int igroup = pdlg->m_page_track.igroup;
		int igroup1 = pdlg->m_page_track.igroup1;
		int idriftx = pdlg->m_page_track.idriftx;
		int idrifty = pdlg->m_page_track.idrifty;
		int igap = pdlg->m_page_track.igap;
		int igap1 = pdlg->m_page_track.igap1;
		int iheight = pdlg->m_page_track.iheight;

		//--2
		if (repeat != 1) {
			goHome();
			goDriftXY();

			// go to led zero
			//m_pMemoryDC->MoveTo(idriftx, rt(idrifty));
			g_MultiCard.MC_ZeroPos(1);
			g_MultiCard.MC_ZeroPos(2);
		
		}

		pulse1.x = 0;
		pulse1.y = 0;
		point1.x = idriftx;
		point1.y = rt(idrifty);

		resetAddons();
	

		for (int i = 0; i < igroup; i++) {
			if (-1 == nextPos('y', igap, vel * 3, point1, pulse1))
				break;
			{
				root["code"] = 1;
				Json::FastWriter writer;
				string msg = writer.write(root);
				theApp.onMessage2SettingMC(msg, MC_MESSAGE);
				pts_time::wait(500);
				if (-1 == nextPos('x', iheight, vel, point1, pulse1))
					break;

				root["code"] = 2;
			//	Json::FastWriter writer;
				 msg = writer.write(root);
				theApp.onMessage2SettingMC(msg, MC_MESSAGE);
			}
			if (-1 == nextPos('y', igap, vel * 3, point1, pulse1))
				break;
			if (-1 == nextPos('x', -1 * iheight, vel * 3, point1, pulse1))
				break;
			
		}
	} while (0);

	movePoint2(15);
	root["code"] = 0;
	Json::FastWriter writer;
	string msg = writer.write(root);
	theApp.onMessage2SettingMC(msg, MC_MESSAGE);

	return;
}
void ThreadMc::handleTracksTask2(Json::Value& command) {
	Json::Value root;
	root["cmd"] = "Track";
	root["sub"] = "step";
	root["dev"] = "mc";
	root["tid"] = 2;
	root["code"] = 0;


	do {
		int repeat = command["repeat"].asInt();
		int vel = command["vel"].asInt();
		if (vel > 100) {
			root["code"] = -1;
			break;
		}
		CMeasureBotDlg* pdlg = (CMeasureBotDlg*)theApp.m_pMainWnd;

		int igroup = pdlg->m_page_track.igroup;
		int igroup1 = pdlg->m_page_track.igroup1;
		int idriftx = pdlg->m_page_track.idriftx;
		int idrifty = pdlg->m_page_track.idrifty;
		int igap = pdlg->m_page_track.igap;
		int igap1 = pdlg->m_page_track.igap1;
		int iheight = pdlg->m_page_track.iheight;

		//--2
		if (repeat != 1) {
			goHome();
			goDriftXY();

			// go to led zero
			//m_pMemoryDC->MoveTo(idriftx, rt(idrifty));
			g_MultiCard.MC_ZeroPos(1);
			g_MultiCard.MC_ZeroPos(2);
		
		}

		if (pdlg->m_page_track.m_use_addons) {
			getAddons(pdlg->m_page_track.m_gapAddon);
		}
		else {
			resetAddons();
		}
	
		pulse2.x = 0;
		pulse2.y = 0;
		point2.x = idriftx;
		point2.y = rt(idrifty);
		for (int i = 0; i < igroup; i++) {

			if (-1 == nextPos('x', igap, vel*2, point2, pulse2,i))
				break;

			{
				root["code"] = 1;
				Json::FastWriter writer;
				string msg = writer.write(root);
				theApp.onMessage2SettingMC(msg, MC_MESSAGE);
				pts_time::wait(500);
				if (-1 == nextPos('y', iheight, vel, point2, pulse2,i,1))
					break;

				root["code"] = 2;
				//Json::FastWriter writer;
				 msg = writer.write(root);
				theApp.onMessage2SettingMC(msg, MC_MESSAGE);
			}
			
			if (-1 == nextPos('x', igap, vel * 2, point2, pulse2,i))
				break;

			if (igroup % 2 == 0 && i + 1 == igroup / 2) {
				if (-1 == nextPos('y', -1 * iheight, vel, point2, pulse2,i))
					break;
			}
			else {
				if (-1 == nextPos('y', -1 * iheight, vel * 2, point2, pulse2))
					break;
			}
			
			/*if (-1 == nextPos('x', igap, vel, point, pulse))
				break;*/

		}
	} while (0);
	movePoint2(15);
	root["code"] = 0;
	Json::FastWriter writer;
	string msg = writer.write(root);
	theApp.onMessage2SettingMC(msg, MC_MESSAGE);

	return;
}
void ThreadMc::handleTracksTask3(Json::Value& command) {

	Json::Value root;
	root["cmd"] = "Track";
	root["sub"] = "step";
	root["tid"] = 3;
	root["dev"] = "mc";
	root["code"] = 0;


	do {
		int repeat = command["repeat"].asInt();
		int vel = command["vel"].asInt();
		if (vel > 100) {
			root["code"] = -1;
			break;
		}
		CMeasureBotDlg* pdlg = (CMeasureBotDlg*)theApp.m_pMainWnd;

		int igroup = pdlg->m_page_track.igroup;
		int igroup1 = pdlg->m_page_track.igroup1;
		int idriftx = pdlg->m_page_track.idriftx;
		int idrifty = pdlg->m_page_track.idrifty;
		int igap = pdlg->m_page_track.igap;
		int igap1 = pdlg->m_page_track.igap1;
		int iheight = pdlg->m_page_track.iheight;

		//--2
		goHome();
		goDriftXY();
		// go to led zero
		//m_pMemoryDC->MoveTo(idriftx, rt(idrifty));
		g_MultiCard.MC_ZeroPos(1);
		g_MultiCard.MC_ZeroPos(2);

		{
			root["code"] = 1;
			Json::FastWriter writer;
			string msg = writer.write(root);
			theApp.onMessage2SettingMC(msg, MC_MESSAGE);
			pts_time::wait(500);
		}
		if (pdlg->m_page_track.m_use_addons) {
			getAddons(pdlg->m_page_track.m_gapAddon);
		}
		else {
			resetAddons();
		}

		CPoint pulse(0, 0);
		CPoint point(idriftx, rt(idrifty));
		CPoint point1(idriftx, rt(idrifty));

		for (int i = 0; i < igroup; i++) {
			
			point1.y = rt(idrifty);
			for (int j = 0; j < igroup1; j++) {
				
				movePoint(vel, point, point1, pulse,j,i);
				root["code"] = 3;
				root["x"] = i;
				root["y"] = j;
				Json::FastWriter writer;
				string msg = writer.write(root);
				theApp.onMessage2SettingMC(msg, MC_MESSAGE);

				pts_time::wait(500);
				point1.y += (igap1);
			}
			point1.x += igap;
		}

	} while (0);

	root["code"] = 0;
	Json::FastWriter writer;
	string msg = writer.write(root);
	theApp.onMessage2SettingMC(msg, MC_MESSAGE);

	return;


}
void ThreadMc::handleTaskPause(Json::Value& command) {

}
void ThreadMc::handleTaskContinue(Json::Value& command) {

}
void ThreadMc::handleTracksMov(Json::Value& command) {

	g_MultiCard.MC_ZeroPos(1);
	g_MultiCard.MC_ZeroPos(2);

	for (int i = 0; i < tracks_.size(); i++)
	{

		int iRes = 0;
		int iAxisNum = tracks_[i].axis;
		g_MultiCard.MC_AxisOn(iAxisNum);
		TTrapPrm TrapPrm;

		int m_lTargetPosPulse = tracks_[i].t * 300; //目标位置 单位脉冲
		TrapPrm.acc = 0.1; //加速度
		TrapPrm.dec = 0.1; //减速度
		TrapPrm.smoothTime = 50; //平滑时间
		TrapPrm.velStart = 0; //启动速度
		int speed = 5; //目标速度

		iRes = g_MultiCard.MC_PrfTrap(iAxisNum);
		iRes += g_MultiCard.MC_SetTrapPrm(iAxisNum, &TrapPrm);

		iRes += g_MultiCard.MC_SetPos(iAxisNum, m_lTargetPosPulse);
		iRes = g_MultiCard.MC_SetVel(iAxisNum, speed);

		iRes += g_MultiCard.MC_Update(0X0001 << (iAxisNum - 1));

		g_MultiCard.MC_AxisOff(iAxisNum);
		DebugLog::writeLogF("track mov: axis=%d  pos=%d speed=%d  ret=%d\n", iAxisNum, m_lTargetPosPulse, speed, iRes);

		int count = 0;
		while (1) {
			pts_time::wait(1000);
			count++;

			TAllSysStatusData m_AllSysStatusDataTemp;
			iRes = g_MultiCard.MC_GetAllSysStatus(&m_AllSysStatusDataTemp);
			double Vel2[2] = { 0.0 };
			g_MultiCard.MC_GetPrfVel(1, Vel2, 2);
			/*double dCrdVel;
			g_MultiCard.MC_GetCrdVel(1, &dCrdVel);*/

			int dstpos = int(m_AllSysStatusDataTemp.dAxisPrfPos[iAxisNum - 1] / 3000);


			DebugLog::writeLogF("track mov: axis=%d  prfpos=%f encpos=%f  ret=%d   ---- pos=%d\n", iAxisNum, m_AllSysStatusDataTemp.dAxisPrfPos[iAxisNum - 1] / 3000,
				m_AllSysStatusDataTemp.dAxisEncPos[iAxisNum - 1] / 3000, iRes, dstpos);

			if (dstpos == tracks_[i].t / 10) break;
			if (count > 30)break;
			int idx = pQueue->peek("jog_stop");
			if (idx >= 0) {
				handleStop(command);
				DebugLog::writeLogF("Track mov: received stop!");
				break;
			}
		}
	}
}


void ThreadMc::handleStop(Json::Value& command)
{
	g_MultiCard.MC_Stop(0XFFFFFFFF, 0XFFFFFFFF);

}


void ThreadMc::handleOpen(Json::Value& command)
{
	int iRes = 0;
	//g_MultiCard.MC_SetCardNo(1);
	Json::Value root;
	Json::FastWriter writer;
	root["cmd"] = "Connect";
	root["dev"] = "mc";

	string hostip = command["ip"].asString();
	string devip = command["devip"].asString();
	int port = command["port"].asInt();

	iRes = g_MultiCard.MC_Open(1, (char*)hostip.c_str(), port, (char*)devip.c_str(), port);
	if (iRes)
	{
		DebugLog::writeLogF("Open Card Fail,Please turn off wifi ,check PC IP address or connection!");
		root["code"] = -1001;
	}
	else
	{
		DebugLog::writeLogF("Open Card Successful!");
		g_MultiCard.MC_SetCommuTimer(3);
		initMc(root);
		root["code"] = 0;
		connected_ = 1;
	}
	string msg = writer.write(root);
	theApp.onMessage2SettingMC(msg, MC_MESSAGE);
	//	theApp.onMessage2RunBoard(msg, MC_MESSAGE);

}
void ThreadMc::resetMc() {
	g_MultiCard.MC_Reset();
}
void ThreadMc::initMc(Json::Value& resp) {
	int iRes = 0;

	//使能电机
	iRes = g_MultiCard.MC_SetExtDoBit(0, 0, 1);
	iRes = g_MultiCard.MC_SetExtDoBit(0, 1, 1);

	//映射硬限位

	iRes = g_MultiCard.MC_SetHardLimP(1, 1, 0, 1);
	iRes = g_MultiCard.MC_SetHardLimN(1, 1, 0, 0);
	iRes = g_MultiCard.MC_SetHardLimP(2, 1, 0, 2);
	iRes = g_MultiCard.MC_SetHardLimN(2, 1, 0, 3);


	//限位常开
	iRes = g_MultiCard.MC_LmtSns(0xFF);
	//零点常开
	iRes = g_MultiCard.MC_HomeSns(0xFF);

	DebugLog::writeLogF("init mc ret=%d",iRes);

	for (int i = 1; i <= 2; i++) {
		int iAxisNum = i;

		//限位使能
		iRes = g_MultiCard.MC_LmtsOn(iAxisNum, MC_LIMIT_POSITIVE);
		iRes = g_MultiCard.MC_LmtsOn(iAxisNum, MC_LIMIT_NEGATIVE);

		short nEncOnOff = 0;
		iRes = g_MultiCard.MC_GetEncOnOff(iAxisNum, &nEncOnOff);

		short nPosLimOnOff = 0;
		short nNegLimOfOff = 0;
		iRes = g_MultiCard.MC_GetLmtsOnOff(iAxisNum, &nPosLimOnOff, &nNegLimOfOff);

		DebugLog::writeLogF("axis %d setting: enc=%d, +lim=%d, -lim=%d\t", i, nEncOnOff, nPosLimOnOff, nNegLimOfOff);

		//退出手轮
		g_MultiCard.MC_EndHandwheel(i);

	}
}

void ThreadMc::handleJogHome(Json::Value& command) {

	int dir = command["dir"].asInt();
	//	int val = command["val"].asInt();
	int iAxisNum = command["axis"].asInt();
	//float acc = (float)command["acc"].asDouble();
	//float dec = command["dec"].asDouble();


	int dir1 = 0;
	int reachlimit = 0;
	int iRes = 0;
	TJogPrm m_JogPrm;

	m_JogPrm.dAcc = 0.1;// acc;// 0.1;
	m_JogPrm.dDec = 0.1;// dec;// 0.1;
	m_JogPrm.dSmooth = 10;

	iRes = g_MultiCard.MC_PrfJog(iAxisNum);
	iRes += g_MultiCard.MC_SetJogPrm(iAxisNum, &m_JogPrm);

	int count = 0;
	while (1) {
		unsigned long lValue = 0;

		{
			int iRes = 0;

			//MC_GetSts
			iRes = g_MultiCard.MC_GetDiRaw(3, (long*)&lValue);
			DebugLog::writeLogF("-------home: ---%ld", lValue);

			if (!(lValue & (1 << (iAxisNum - 1)))) {
				g_MultiCard.MC_ZeroPos(iAxisNum);
				iRes = g_MultiCard.MC_ClrSts(iAxisNum);
				if (iAxisNum == 1) {
					g_MultiCard.MC_Stop(0X1, 0X1);
				}
				else {
					g_MultiCard.MC_Stop(0X2, 0X2);
				}
				DebugLog::writeLogF("------go-home OK");

				break;
			}

			poslimit1 = 0;
			neglimit1 = 0;
			poslimit2 = 0;
			neglimit2 = 0;

			//轴状态显示
			lValue = 0;

			//通用输入IO信号显示
			iRes = g_MultiCard.MC_GetExtDiValue(0, &lValue, 1);
			if (!(lValue & 1)) {
				neglimit1 = 1;
			}
			if (!(lValue & 1 << 1)) {
				poslimit1 = 1;
			}
			if (!(lValue & 1 << 2)) {
				poslimit2 = 1;
			}
			if (!(lValue & 1 << 3)) {
				neglimit2 = 1;
			}

			DebugLog::writeLogF("-------limit: ---%ld %d %d %d %d    axis=%d dir=%d", lValue, poslimit1, neglimit1, poslimit2, neglimit2, iAxisNum, dir);

		}

		if (iAxisNum == 1 && ((poslimit1 == 1 && dir == 1) || (neglimit1 == 1 && dir == -1))) {
			g_MultiCard.MC_Stop(0X1, 0X1);
			reachlimit = 1;
			//continue;
		}
		else if (iAxisNum == 2 && ((poslimit2 == 1 && dir == 1) || (neglimit2 == 1 && dir == -1))) {
			g_MultiCard.MC_Stop(0X2, 0X2);
			reachlimit = 1;
			//continue;
		}


		count++;
		if (count > 500)
		{
			DebugLog::writeLogF("move safe mov: break!");

			break;
		}

		if (dir1 == 0) {
			if (reachlimit == 1) {
				dir1 = 0 - dir;
				dir = dir1;
				DebugLog::writeLogF("====================move dir change : break!");

			}

		}
		iRes += g_MultiCard.MC_SetVel(iAxisNum, dir * 10.0);
		iRes += g_MultiCard.MC_Update(0X0001 << (iAxisNum - 1));

		if (iRes == 0) {
			DebugLog::writeLogF("Jog mov: tm=%ld, axis = %d dir=%d reach=%d", pts_time::currentms(), iAxisNum, dir, reachlimit);
		}
		else {
			DebugLog::writeLogF("Jog mov: failed!");

		}
		int idx = pQueue->peek("jog_stop");
		if (idx >= 0) {
			handleStop(command);
			DebugLog::writeLogF("Jog mov: received stop!");
			break;
		}

		pts_time::wait(100);

	}
}

void ThreadMc::handleJogMov(Json::Value& command) {

	int dir = command["dir"].asInt();
	int val = command["val"].asInt();
	int iAxisNum = command["axis"].asInt();
	float acc = (float)command["acc"].asDouble();
	float dec = command["dec"].asDouble();

	//seq
	//timestamp


	int iRes = 0;
	TJogPrm m_JogPrm;

	m_JogPrm.dAcc = acc;// 0.1;
	m_JogPrm.dDec = dec;// 0.1;
	m_JogPrm.dSmooth = 0;

	iRes = g_MultiCard.MC_PrfJog(iAxisNum);

	iRes += g_MultiCard.MC_SetJogPrm(iAxisNum, &m_JogPrm);

	iRes += g_MultiCard.MC_SetVel(iAxisNum, dir * 20.0);

	int count = 0;
	while (1) {
		count++;
		if (count > 100) break;

		/*	if (iAxisNum == 1 && ((poslimit1 == 1 && dir == 1) || (neglimit1 == 1 && dir == -1))) {
				g_MultiCard.MC_Stop(0X1, 0X1);
				break;
			}
			else if (iAxisNum == 2 && ((poslimit2 == 1 && dir == 1) || (neglimit2 == 1 && dir == -1))) {
				g_MultiCard.MC_Stop(0X2, 0X2);
				break;
			}*/
		iRes += g_MultiCard.MC_Update(0X0001 << (iAxisNum - 1));

		if (iRes == 0) {
			DebugLog::writeLogF("Jog mov: tm=%ld, axis = %d", pts_time::currentms(), iAxisNum);
		}
		else {
			DebugLog::writeLogF("Jog mov: failed! %d", iRes);

		}

		pts_time::wait(50);
		g_MultiCard.MC_Stop(0X2, 0X2);
		int idx = pQueue->peek("jog_stop");
		if (idx >= 0) {
			handleStop(command);
			DebugLog::writeLogF("Jog mov: received stop!");
			break;
		}

	}
}

void ThreadMc::handleZero(Json::Value& command) {
	int iaxis = command["axis"].asInt();
	if (iaxis == 1 || iaxis == 3) {
		//g_MultiCard.MC_ClrSts(1);
		g_MultiCard.MC_ZeroPos(1);

	}
	if (iaxis == 2 || iaxis == 3) {
		//g_MultiCard.MC_ClrSts(2);
		g_MultiCard.MC_ZeroPos(2);

	}
	handPulse.x = 0;
	handPulse.y = 0;
	
}

void ThreadMc::handleMovPos(Json::Value& command)
{

	int xdist = command["xdist"].asInt();
	int ydist = command["ydist"].asInt();
	int vel = command["vel"].asInt();

	CPoint point(0, rt(0));
	CPoint point1(xdist, rt(ydist));

//	g_MultiCard.MC_ClrSts(1,2);
	g_MultiCard.MC_ZeroPos(1,2);
	handPulse.x = 0;
	handPulse.y = 0;

	movePoint1(vel, point, point1, handPulse);

}
