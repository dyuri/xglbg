#ifndef _UTIL_H
#define _UTIL_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>

#include "config.h"

typedef struct ImgInfo {
  int width, height, nrChannels;
  char *imageDir;
  struct dirent **imgList;
  int imgNum;
  int currentImgIdx;

  float dayProgress;
  float imageProgress;

  int *cont;
} ImgInfo;

float getUnixTime();
float getDayProgress();

int endsWith(const char* str, const char* suffix);
int isPng(const struct dirent *entry);

#endif
