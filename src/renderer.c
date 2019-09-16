#include "renderer.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

renderer *init_rend()
{
  renderer *r = (renderer *)malloc(sizeof(struct renderer));

  r->win = init_xwin();

  int gl3attr[] = {
    GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
    GLX_CONTEXT_MINOR_VERSION_ARB, 0,
    GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
    GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
    None
  };

  r->ctx = glXCreateContextAttribsARB(r->win->display, r->win->fbc, NULL, True, gl3attr);

  if (!r->ctx) {
    printf("Couldn't create an OpenGL context\n");
    exit(1);
  }

  XTextProperty windowName;
  windowName.value = (unsigned char *) "Live wallpaper";
  windowName.encoding = XA_STRING;
  windowName.format = 8;
  windowName.nitems = strlen((char *) windowName.value);

  XSetWMName(r->win->display, r->win->window, &windowName);

  XMapWindow(r->win->display, r->win->window);
  glXMakeCurrent(r->win->display, r->win->window, r->ctx);

  if (cfg.debug) {
    printf("OpenGL:\n\tvendor %s\n\trenderer %s\n\tversion %s\n\tshader language %s\n",
        glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION),
        glGetString(GL_SHADING_LANGUAGE_VERSION)
    );
  }

  int extCount;
  glGetIntegerv(GL_NUM_EXTENSIONS, &extCount);
  Bool found = False;
  for (int i = 0; i < extCount; ++i)
    if (!strcmp((const char*)glGetStringi(GL_EXTENSIONS, i), "GL_ARB_compute_shader")) {
      if (cfg.debug)
        printf("Extension \"GL_ARB_compute_shader\" found\n");
      found = True;
      break;
    }

  if (!found) {
    printf("Extension \"GL_ARB_compute_shader\" not found\n");
    exit(1);
  }

  glViewport(0, 0, r->win->width, r->win->height);

  checkErrors("Window init");

  if (cfg.debug) {
    printf("DayProgress: %f\n", getDayProgress());
    printf("Config dir: %s\n", getConfigRoot());
    printf("Image dir: %s\n", getImageDir());
    printf("Shader dir: %s\n", getShaderDir());
  }

  return r;
}

void linkBuffers(renderer *r)
{
  glUseProgram(r->progID);

  glGenVertexArrays(1, &vertArray);
  glBindVertexArray(vertArray);

  glGenBuffers(1, &posBuf);
  glBindBuffer(GL_ARRAY_BUFFER, posBuf);
  float data[] = {
    -1.0f, -1.0f,
    -1.0f, 1.0f,
    1.0f, -1.0f,
    1.0f, 1.0f
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8, data, GL_STREAM_DRAW);
  GLint posPtr = glGetAttribLocation(r->progID, "pos");
  glVertexAttribPointer(posPtr, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(posPtr);

  checkErrors("Linking buffers");

  glGenTextures(1, &r->audioSamples);
  glBindTexture(GL_TEXTURE_1D, r->audioSamples);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glGenTextures(1, &r->audioFFT);
  glBindTexture(GL_TEXTURE_1D, r->audioFFT);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glGenTextures(1, &r->image);
  glBindTexture(GL_TEXTURE_2D, r->image);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenTextures(1, &r->nextimage);
  glBindTexture(GL_TEXTURE_2D, r->nextimage);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glUniform1i(glGetUniformLocation(r->progID, "samples"), 0);
  glUniform1i(glGetUniformLocation(r->progID, "fft"), 1);
  glUniform1i(glGetUniformLocation(r->progID, "image"), 2);
  glUniform1i(glGetUniformLocation(r->progID, "nextimage"), 3);

  checkErrors("Linking textures");
}

void loadImage(ImgInfo* imgInfo, char* name, GLuint target, GLenum texture)
{
  unsigned char *image;

  stbi_set_flip_vertically_on_load(true);
  char* imgName = (char*)malloc((strlen(imgInfo->imageDir) + strlen(cfg.imageTheme) + strlen(name) + 2) * sizeof(char));
  sprintf(imgName, "%s/%s/%s", imgInfo->imageDir, cfg.imageTheme, name);
  image = stbi_load(imgName, &imgInfo->width, &imgInfo->height, &imgInfo->nrChannels, 0);

  if (image) {
    glActiveTexture(texture);
    glBindTexture(GL_TEXTURE_2D, target);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgInfo->width, imgInfo->height, 0, imgInfo->nrChannels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, image);
    if (cfg.debug)
      printf("Image loaded: %s [%d:%d @%d]\n", imgName, imgInfo->width, imgInfo->height, imgInfo->nrChannels);
  } else {
    if (cfg.debug)
      printf("Couldn't load image\n");
    glActiveTexture(texture);
    glBindTexture(GL_TEXTURE_2D, target);

    unsigned char tmp[4] = {255, 255, 255, 255};

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp);
  }

  stbi_image_free(image);
  free(imgName);
}

void updateImgInfo(renderer* r) {
  r->imgInfo.dayProgress = getDayProgress();

  float progress = r->imgInfo.dayProgress * r->imgInfo.imgNum * cfg.imageSpeed;
  int imgIdx = (int)progress;
  r->imgInfo.imageProgress = progress - imgIdx;

  imgIdx = imgIdx % r->imgInfo.imgNum;

  // Load image if toggled
  if (imgIdx != r->imgInfo.currentImgIdx) {
    r->imgInfo.currentImgIdx = imgIdx;
    int nextIdx = (imgIdx + 1) % r->imgInfo.imgNum;

    if (cfg.debug)
      printf("Image index: %d, next: %d\n", imgIdx, nextIdx);

    loadImage(&r->imgInfo, r->imgInfo.imgList[imgIdx]->d_name, r->image, GL_TEXTURE2);
    loadImage(&r->imgInfo, r->imgInfo.imgList[nextIdx]->d_name, r->nextimage, GL_TEXTURE3);
  }

}

void render(renderer *r, float *sampleBuff, float *fftBuff, int buffSize)
{
  // Clear screen
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);

  // Configure & link opengl
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_CONSTANT_ALPHA);

  glUseProgram(r->progID);
  glBindVertexArray(vertArray);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_1D, r->audioSamples);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_R16, buffSize, 0, GL_RED, GL_FLOAT, sampleBuff);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, r->audioFFT);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_R16, buffSize/2+1, 0, GL_RED, GL_FLOAT, fftBuff);

  updateImgInfo(r);

  // Set uniforms for shaders
  GLint timeLoc = glGetUniformLocation(r->progID, "time");
  if (timeLoc != -1) glUniform1f(timeLoc, getUnixTime());

  GLint dayProgLoc = glGetUniformLocation(r->progID, "dayprogress");
  if (dayProgLoc != -1) glUniform1f(dayProgLoc, r->imgInfo.dayProgress);

  GLint imageProgLoc = glGetUniformLocation(r->progID, "imageprogress");
  if (imageProgLoc != -1) glUniform1f(imageProgLoc, r->imgInfo.imageProgress);

  GLint nrImagesLoc = glGetUniformLocation(r->progID, "nrimages");
  if (nrImagesLoc != -1) glUniform1f(nrImagesLoc, r->imgInfo.imgNum);

  GLint resolutionLoc = glGetUniformLocation(r->progID, "resolution");
  if (resolutionLoc != -1) glUniform2f(resolutionLoc, (float)r->win->width, (float)r->win->height);

  // Draw screen
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  checkErrors("Draw screen");
  swapBuffers(r->win);
}

void checkErrors(const char *desc) {
  GLenum e = glGetError();
  if (e != GL_NO_ERROR) {
    printf("OpenGL error in \"%s\": %s (%d)\n", desc, gluErrorString(e), e);
    exit(1);
  }
}
