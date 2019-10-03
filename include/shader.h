#ifndef _SHADER_H
#define _SHADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opengl.h"

static const char *vertCodeDef =
  "#version 430\n\
  in vec2 pos;\
  void main() {\
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);\
  }";

static const char *fragCodeDef =
  "#version 430\n\
  uniform vec2 resolution;\
  uniform float time;\
  uniform sampler1D fft;\
  out vec4 color;\
  void main()\
  {\
    vec2 uv = gl_FragCoord.xy / resolution.xy;\
    float f = texelFetch(fft, int(uv.x * 100), 0).r;\
    color = vec4(sin(time), cos(time / 2.0), 1. - step(f, uv.y), 0.25);\
  }";

void checkCompileErrors(unsigned int shader, const char *type);

GLuint loadShaders(const char *vertPath, const char *fragPath)
{
  GLuint prog = glCreateProgram();
  GLuint vert = glCreateShader(GL_VERTEX_SHADER);
  GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

  char *vertCode;
  char *fragCode;
  int vertSize = 0;
  int fragSize = 0;

  FILE* vertFile = fopen(vertPath, "r");
  if (vertFile) {
    fseek(vertFile, 0, SEEK_END);
    vertSize = ftell(vertFile);
    fseek(vertFile, 0, SEEK_SET);

    vertCode = (char *)malloc(vertSize + 1);
    fread(vertCode, vertSize, 1, vertFile);

    if (cfg.debug)
      printf("Vertex shader loaded from: %s\n", vertPath);
  } else {
    if (cfg.debug)
      printf("Couldn't load vertex shader (%s), using default code\n", vertPath);

    vertCode = (char *)vertCodeDef;
    vertSize = strlen(vertCode);
  }

  FILE* fragFile = fopen(fragPath, "r");
  if (fragFile) {
    fseek(fragFile, 0, SEEK_END);
    fragSize = ftell(fragFile);
    fseek(fragFile, 0, SEEK_SET);

    fragCode = (char *)malloc(fragSize + 1);
    fread(fragCode, fragSize, 1, fragFile);

    if (cfg.debug)
      printf("Fragment shader loaded from: %s\n", fragPath);
  } else {
    if (cfg.debug)
      printf("Couldn't load fragment shader (%s), using default code\n", fragPath);
    fragCode = (char *)fragCodeDef;
    fragSize = strlen(fragCode);
  }

  glShaderSource(vert, 1, (const char **)&vertCode, &vertSize);
  glShaderSource(frag, 1, (const char **)&fragCode, &fragSize);

  glCompileShader(vert);
  checkCompileErrors(vert, "VERTEX");
  glAttachShader(prog, vert);

  glCompileShader(frag);
  checkCompileErrors(frag, "FRAGMENT");
  glAttachShader(prog, frag);

  glBindFragDataLocation(prog, 0, "color");
  glLinkProgram(prog);
  checkCompileErrors(prog, "PROGRAM");

  return prog;
}

void checkCompileErrors(unsigned int shader, const char *type)
{
  int success;
  char infoLog[1024];
  if (strcmp(type, "PROGRAM"))
  {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      printf("ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s\n", type, infoLog);
    }
  }
  else
  {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      printf("ERROR::SHADER_LINKING_ERROR of type: %s\n%s\n", type, infoLog);
    }
  }
}

#endif
