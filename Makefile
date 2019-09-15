all:
	clang -o xglbg src/* -Wall -Iinclude -lm -lpthread -lfftw3 -lpulse-simple -lpulse -lGL -lGLU -lX11 -lXrender -lXfixes -g
