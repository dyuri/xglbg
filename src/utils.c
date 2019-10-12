#include "utils.h"

float getUnixTime()
{
	struct timespec tv;
	clock_gettime(CLOCK_REALTIME_COARSE, &tv);
	return (float)((int)tv.tv_sec % 1000000) + (float)(tv.tv_nsec) / 1.0e9;
}

float getDayProgress()
{
	struct tm * timeinfo;
  float dp;

	struct timespec tv;
  time_t rawtime;
	clock_gettime(CLOCK_REALTIME_COARSE, &tv);

  rawtime = (time_t)tv.tv_sec;
	timeinfo = localtime(&rawtime);

  dp = (float)timeinfo->tm_hour / 24 + (float)timeinfo->tm_min / (24 * 60) + (float)timeinfo->tm_sec / (24 * 60 * 60) + (float)tv.tv_nsec / (24 * 60 * 60 * 1.0e9);

	return dp;
}

int endsWith(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

int isPng(const struct dirent *entry)
{
  return endsWith(entry->d_name, ".png") || endsWith(entry->d_name, ".jpg");
}
