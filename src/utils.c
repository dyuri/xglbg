#include "utils.h"

float getUnixTime()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return (float)((int)tv.tv_sec % 1000000) + (float)(tv.tv_usec) / 1.0e6;
}

char *getSystemTime()
{
	char *format;

	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	format = (char *)malloc(16);

	sprintf(format, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

	return format;
}

int exec(char *cmd, char *buf, int size)
{
	FILE *fp;

	fp = popen(cmd, "r");
	if (fp == NULL) {
		return -1;
	}

	fgets(buf, size, fp);

	pclose(fp);

    if (buf[strlen(buf) - 1] == '\n')
        buf[strlen(buf) - 1] = '\0';

	return 0;
}
