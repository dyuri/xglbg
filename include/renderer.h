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
  GLuint progID, progText;
  GLuint audioSamples;
  GLuint audioFFT;
  GLXContext ctx;

  GLuint image;
  ImgInfo imgInfo;
} renderer;

GLuint vertArray, posBuf;

typedef struct character {
  GLuint	textureID;
  float	sizeX, sizeY;
  float	bearingX, bearingY;
  GLuint	advance;
} character;

character *characters[65536];

GLuint VAO, VBO;

renderer *init_rend();
void linkBuffers(renderer *r);
void render(renderer *r, float *sampleBuff, float *fftBuff, int buffSize);

#endif
