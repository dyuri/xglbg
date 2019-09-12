#include "config.h"

void printHelp()
{
    printf("Usage: liveW <options>                                                      \n");
    printf("Options:                                                                    \n");
    printf("   -h Print help                                                            \n");
    printf("   -d Turn debug on                                                         \n");
    printf("   -k Plasma window setup                                                   \n");
    printf("   -w Window mode                                                           \n");
    printf("   -g Window geometry WIDTHxHEIGHT (default 800x600)                        \n");
    printf("   -o Position of window relative to top left corner TOPxLEFT (default 0x0) \n");
    printf("   -t Transparency (default 1.0)                                            \n");
    printf("   -p shader name in shaders folder                                         \n");
    printf("   -f FPS (default 30)                                                      \n");
    printf("   -D Dynamic FPS - use 1 FPS if there is no sound                          \n");
    printf("   -s Pulseaudio device source                                              \n");
    printf("      Specify using the name from \"pacmd list-sources | grep \"name:\"\"   \n");
}

bool parseArgs(int argc, char *argv[])
{
    cfg.debug = false;
    cfg.src = NULL;
    cfg.plasma = false;
    cfg.windowed = false;
    cfg.geometry = false;
    cfg.offX = 0;
    cfg.offY = 0;
    cfg.width = 800;
    cfg.height = 600;
    cfg.transparency = 1.0;
    cfg.shaderName = NULL;
    cfg.fps = 30;

    char c;
    const char *opt_str = "hdkwDYs:t:g:o:f:p:F:";
    while ((c = getopt(argc, argv, opt_str)) != -1) {
        switch (c) {
            case 'h':
                return false;
                break;
            case 'd':
                cfg.debug = true;
                break;
            case 'k':
                cfg.plasma = true,
                cfg.windowed = true;
                break;
            case 'w':
                cfg.windowed = true;
                break;
            case 's':
                cfg.src = strdup(optarg);
                break;
            case 't':
                sscanf(optarg, "%f", &cfg.transparency);
                break;
            case 'g':
                sscanf(optarg, "%fx%f", &cfg.width, &cfg.height);
                cfg.geometry = true;
                break;
            case 'o':
                sscanf(optarg, "%fx%f", &cfg.offX, &cfg.offY);
                cfg.geometry = true;
                break;
            case 'p':
                cfg.shaderName = strdup(optarg);
                break;
            case 'f':
                sscanf(optarg, "%u", &cfg.fps);
                break;
            case 'D':
                cfg.dontDrawIfNoSound = true;
                break;
            default:
                return false;
                break;
        }
    }
    return true;
}

char* getHomeDir()
{
  char* homedir;
  if ((homedir = getenv("HOME")) == NULL) {
    homedir = getpwuid(getuid())->pw_dir;
  }

  return homedir;
}

char* getConfigRoot()
{
  char* configroot;

  if ((configroot = getenv("XDG_CONFIG_HOME")) == NULL) {
    char* home = getHomeDir();
    char* buf = (char*)malloc((strlen(home) + strlen("/.config")) * sizeof(char));
    sprintf(buf, "%s%s", home, "/.config"); 
    configroot = buf;
  }

  char* configdir = (char*)malloc((strlen(configroot) + strlen("/xglbg")) * sizeof(char));
  sprintf(configdir, "%s%s", configroot, "/xglbg");

  // create if not exsists
  struct stat st = {0};

  if (stat(configdir, &st) == -1) {
    mkdir(configdir, 0755);
  }

  return configdir;
}

char* getShaderDir()
{
  return getConfigRoot();
}

char* getImageDir()
{
  return getConfigRoot();
}
