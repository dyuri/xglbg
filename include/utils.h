#ifndef _UTIL_H
#define _UTIL_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "config.h"

typedef struct ImgInfo {
  bool newImg;
  int width, height, nrChannels;
  unsigned char *image;
  char *imageDir;
  float dayProgress;

  int *cont;
} ImgInfo;

float getUnixTime();
float getDayProgress();

#endif
