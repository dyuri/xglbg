#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <locale.h>
#include <dirent.h>

#include "utils.h"
#include "config.h"
#include "shader.h"
#include "renderer.h"
#include "pulsefft.h"

static struct pa_fft *ctx;

void sig_handler(int sig_no) {
    if (sig_no == SIGINT) {
        printf("\rExiting\n");
        ctx->cont = 0;
    }
}

int main(int argc, char *argv[])
{
    // Handle Ctrl+C
    struct sigaction action;
    action.sa_handler = &sig_handler;
    sigaction(SIGINT, &action, NULL);

    // Parse arguments
    if (!parseArgs(argc, argv)) {
        printHelp();
        return 0;
    }

    // Set locale
    setlocale(LC_ALL, "");

    // Init renderer
    renderer *rend = init_rend();

    // Load shaders
    char *vertPath = NULL, *fragPath = NULL;
    if (cfg.shaderName) {
      char *shaderRoot = getShaderDir();
      vertPath = (char*)malloc((strlen(shaderRoot) + strlen("//vert.glsl") + strlen(cfg.shaderName)) * sizeof(char));
      fragPath = (char*)malloc((strlen(shaderRoot) + strlen("//frag.glsl") + strlen(cfg.shaderName)) * sizeof(char));
      sprintf(vertPath, "%s/%s/vert.glsl", shaderRoot, cfg.shaderName);
      sprintf(fragPath, "%s/%s/frag.glsl", shaderRoot, cfg.shaderName);
    }
    rend->progID = loadShaders(vertPath, fragPath);

    // Load image list
    if (cfg.imageTheme) {
      char *imageDir = getImageDir();
      char *imageThemeDir = (char*)malloc((strlen(imageDir) + strlen(cfg.imageTheme) + 1) * sizeof(char));
      sprintf(imageThemeDir, "%s/%s", imageDir, cfg.imageTheme);

      if (cfg.debug)
        printf("Image theme directory: %s\n", imageThemeDir);

      rend->imgInfo.imgNum = scandir(imageThemeDir, &rend->imgInfo.imgList, isPng, alphasort);
      if (cfg.debug) {
        if (rend->imgInfo.imgNum < 0)
          printf("Cannot list files in %s\n", imageThemeDir);
        else {
          for (int i=0; i<rend->imgInfo.imgNum; i++) {
            printf("%s\n", rend->imgInfo.imgList[i]->d_name);
          }
        }
      }
    }

    // Configure fft
    ctx = calloc(1, sizeof(struct pa_fft));
    ctx->cont = 1;
    ctx->samples = 2048;
    ctx->dev = cfg.src;
    ctx->rate = 44100;
    rend->imgInfo.cont = &ctx->cont;
    rend->imgInfo.imageDir = getImageDir();
    rend->imgInfo.currentImgIdx = -1;

    // Init pulseaudio && fft
    init_pulse(ctx);
    init_buffers(ctx);
    init_fft(ctx);

    linkBuffers(rend);

    // Create threads for pulseaudio & getting song info
    pthread_create(&ctx->thread, NULL, pa_fft_thread, ctx);

    unsigned int fps = cfg.fps;

    while(ctx->cont) {
        render(rend, ctx->pa_output, ctx->fft_output, ctx->samples);

        // Doesn't render if there is no sound
        if (cfg.dontDrawIfNoSound) {
          bool noNewSound = true;
          for (int i = 0; i < ctx->samples; i++)
            if (*(ctx->pa_output + i))
              noNewSound = false;

          if (noNewSound) {
            fps = 1;
          } else {
            fps = cfg.fps;
          }
        }

        usleep(1000000 / fps);
    }

    usleep(1000000 / cfg.fps);

    deinit_fft(ctx);

    return 0;
}
