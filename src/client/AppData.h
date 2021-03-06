#ifndef APPDATA_H
#define APPDATA_H


#ifdef __linux
#include <X11/Xlib.h>
#endif
#include "../common.h"
#include "../Wnd.h"


#define SIZEX 300
#define SIZEY 200

typedef struct
{
	#ifdef __linux
	Display *disp;
	Wnd popWnd;
	int numWnd;
	#endif
} AppData;

#endif
