#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <stdio.h>

struct config {
    int debug;
    char *src;
    bool plasma;
    bool windowed;
    bool geometry;
    float offX, offY;
    float width, height;
    float transparency;
    char *shaderName;
    char *imageTheme;
    float imageSpeed;
    unsigned int fps;
    bool dontDrawIfNoSound;
    bool mono;
} cfg;

void printHelp();
bool parseArgs(int argc, char *argv[]);
char* getHomeDir();
char* getConfigRoot();
char* getShaderDir();
char* getImageDir();

#endif // _CONFIG_H_
