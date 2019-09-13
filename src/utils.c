#include "utils.h"

float getUnixTime()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return (float)((int)tv.tv_sec % 1000000) + (float)(tv.tv_usec) / 1.0e6;
}

float getDayProgress()
{
	time_t rawtime;
	struct tm * timeinfo;
  float dp;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

  dp = (float)timeinfo->tm_hour / 24 + (float)timeinfo->tm_min / (24 * 60) + (float)timeinfo->tm_sec / (24 * 60 * 60);

	return dp;
}

