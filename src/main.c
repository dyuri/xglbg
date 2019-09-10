#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <locale.h>

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
        vertPath = (char*)malloc((strlen("shaders//vert.glsl") + strlen(cfg.shaderName)) * sizeof(char));
        fragPath = (char*)malloc((strlen("shaders//frag.glsl") + strlen(cfg.shaderName)) * sizeof(char));
        sprintf(vertPath, "shaders/%s/vert.glsl", cfg.shaderName);
        sprintf(fragPath, "shaders/%s/frag.glsl", cfg.shaderName);
    }
    rend->progID = loadShaders(vertPath, fragPath);

    // Load text shaders
    rend->progText = loadShaders("shaders/text/vert.glsl", "shaders/text/frag.glsl");

    // Configure fft
    ctx = calloc(1, sizeof(struct pa_fft));
    ctx->cont = 1;
    ctx->samples = 2048;
    ctx->dev = cfg.src;
    ctx->rate = 44100;
    rend->imgInfo.cont = &ctx->cont;
    rend->imgInfo.newImg = true;

    // Init pulseaudio && fft
    init_pulse(ctx);
    init_buffers(ctx);
    init_fft(ctx);

    linkBuffers(rend);

    // Create threads for pulseaudio & getting song info
    pthread_create(&ctx->thread, NULL, pa_fft_thread, ctx);

    while(ctx->cont) {
        render(rend, ctx->pa_output, ctx->fft_output, ctx->samples);

        usleep(1000000 / cfg.fps);
    }

    usleep(1000000 / cfg.fps);

    deinit_fft(ctx);

    return 0;
}
