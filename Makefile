XDG_CONFIG_HOME ?= $(HOME)/.config
all:
	clang -o xglbg src/* -Wall -Iinclude -lm -lpthread -lfftw3 -lpulse-simple -lpulse -lGL -lGLU -lX11 -lXrender -lXfixes -g

config:
	mkdir -p $(XDG_CONFIG_HOME)/xglbg
	cp -r shaders images $(XDG_CONFIG_HOME)/xglbg
