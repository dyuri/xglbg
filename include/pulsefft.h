#ifndef _PULSEFFT_H
#define _PULSEFFT_H

#include <stdio.h>
#include <complex.h>
#include <tgmath.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>

#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/pulseaudio.h>

#include <fftw3.h>
#include "config.h"

typedef struct pa_fft {
    pthread_t thread;
    int cont;

    // Pulseaudio
    pa_simple *s;
    char *dev;
    unsigned int rate;
    int error;
    pa_sample_spec ss;
    pa_channel_map map;

    // Buffers
    unsigned int samples;
    
    double *pa_buff_l;
    double *pa_buff_r;
    size_t pa_buff_size;
    unsigned int pa_samples;

    double *fft_buff;
    double *fft_buff_l;
    double *fft_buff_r;
    size_t fft_buff_size;
    unsigned int fft_samples;

    // Output buffers
    float *pa_output;
    size_t pa_output_size;
    
    float *fft_output;
    size_t fft_output_size;

    // FFTW
    fftw_complex *output_l;
    fftw_complex *output_r;
    size_t output_size;
    unsigned int output_samples;
    fftw_plan plan_l;
    fftw_plan plan_r;
} pa_fft;

void separate_freq_bands(double *out, fftw_complex *in, int n, int *lcf, int *hcf, float *k, double sensitivity, int in_samples);
void *pa_fft_thread(void *arg);
void get_pulse_default_sink(pa_fft* pa_fft);
void init_pulse(pa_fft *pa_fft);
void init_buffers(pa_fft *pa_fft);
void init_fft(pa_fft *pa_fft);
void deinit_fft(pa_fft *pa_fft);

#endif // _PULSEFFT_H
