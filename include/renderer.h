#ifndef _RENDERER_H
#define _RENDERER_H
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "xwin.h"
#include "utils.h"
#include "config.h"

typedef struct renderer {
  xwin *win;
  GLuint progID;
  GLuint audioSamples;
  GLuint audioFFT;
  GLXContext ctx;

  GLuint image;
  GLuint nextimage;
  ImgInfo imgInfo;
} renderer;

GLuint vertArray, posBuf;

renderer *init_rend();
void linkBuffers(renderer *r);
void render(renderer *r, float *sampleBuff, float *fftBuff, int buffSize, bool noNewSound);

#endif
