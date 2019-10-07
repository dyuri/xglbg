XDG_CONFIG_HOME?=$(HOME)/.config
COMPILER?=clang
CFLAGS=-g -Wall -Iinclude `pkg-config --cflags fftw3 --cflags glu --cflags xrender --cflags xfixes --cflags libpulse-simple`
LDFLAGS=-lm -lpthread `pkg-config --libs fftw3 --libs glu --libs xrender --libs xfixes --libs libpulse-simple`
all:
	$(COMPILER) -o xglbg src/* $(CFLAGS) $(LDFLAGS)

config:
	mkdir -p $(XDG_CONFIG_HOME)/xglbg
	cp -r shaders images $(XDG_CONFIG_HOME)/xglbg
