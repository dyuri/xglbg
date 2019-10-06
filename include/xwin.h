#ifndef _XWIN_H
#define _XWIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xrender.h>

#include "opengl.h"
#include "config.h"

#define MAX_PROPERTY_VALUE_LEN 4096

typedef struct xwin {
	Display *display;
	Window root;
	Window window;
	Window desktop;
	GLint screenNum;

	XSetWindowAttributes swa;
	GLXFBConfig fbc, *fbcs;
	XVisualInfo *vi;
	XRenderPictFormat *pict;
	Colormap cmap;

  unsigned int window_count;
  unsigned int desktop_count;
  int current_desktop;
  unsigned int current_desktop_window_count;

	GLint offX, offY;
	GLint width, height;
} xwin;

xwin *init_xwin();
void swapBuffers();
char *get_property (Display *disp, Window win, Atom xa_prop_type, char *prop_name, unsigned long *size);
void update_window_list(xwin* win);

#endif

