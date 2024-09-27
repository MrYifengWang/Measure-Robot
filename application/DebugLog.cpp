

#include <pch.h>

#include "DebugLog.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include "framework.h"

#define LogName "D:\\MeasureBot\\proj\\MeasureBot\\log\\MeasureBot.log"
string m_tRoot;
pts_mutex lock_log;

//写日志文件
DebugLog::DebugLog()
{
	// TODO Auto-generated constructor stub

}

DebugLog::~DebugLog()
{
	// TODO Auto-generated destructor stub
}

void DebugLog::init() {

	{
		char path[512];
		getcwd(path, 511);

		m_tRoot = path;
		m_tRoot += "/logs";

		struct stat buffer;

		if (stat(m_tRoot.c_str(), &buffer) != 0) {
			mkdir(m_tRoot.c_str());
		}
	}
}
void DebugLog::writefile(char* log)
{
	lock_log.lock();
	char pStrTime[64] = { 0 };
	time_t t;
	time(&t);
	tm* local = localtime(&t);
	strftime(pStrTime, 64, "\n[%Y-%m-%d %H:%M:%S] - ", local);


	int year, month, day, hour, minute, second;

	time_t current;
	time(&current);

	struct tm* l = localtime(&current);

	year = l->tm_year + 1900;
	month = l->tm_mon + 1;
	day = l->tm_mday;
	/*hour = l->tm_hour;
	minute = l->tm_min;
	second = l->tm_sec;*/

	stringstream path("");
	path << m_tRoot << "/" << year << "-" << month << "-" << day << ".txt";

	FILE* file = fopen(path.str().c_str(), "a");
	int flen = ftell(file);

	if (file)
	{
		fwrite(pStrTime, strlen(pStrTime), 1, file);
		fwrite(log, strlen(log), 1, file);
		fclose(file);
	}
	if (flen > 1024 * 1024)
	{
		char buf[128] = {0};// "del debug.log";
		sprintf(buf, "del %s", LogName);
		system(buf);
	}

	lock_log.unlock();
}

void DebugLog::writeLog(char* log)
{
	writefile(log);
}
void DebugLog::writeLogF(const char *fmt, ...)
{
	va_list ap;
	char msg[LOG_MAX_MSG_LEN];
	va_start(ap, fmt);
	vsnprintf(msg, sizeof(msg), fmt, ap);
	writefile(msg);
	va_end(ap);

}

