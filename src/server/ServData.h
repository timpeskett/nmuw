#ifndef SERVDATA_H
#define SERVDATA_H

#include "../common.h"
#include "Monitor.h"
#include "../Wnd.h"
#include <string.h>
#include <stdlib.h>
#ifdef __linux
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#define MAX_MON 10
#define MASTER 0 

typedef struct
{
	#ifdef __linux
	Display *disp;
	Monitor *mons[MAX_MON];
	int pointerx, pointery; /* Position of pointer (across ALL monitors) */
	int focusMon;
	int curMon, lastFocus; /* Number of monitor with keyboard focus. curFocus is required in addition to focusMon because when the user opens the application window the focus will change to the MASTER monitor, but we don't want that displayed first in our list of monitors. lastFocus is the previous monitor that was active, set to MAX_MON if not yet applicable */
	Wnd appWnd, listWnd;	
	Wnd captWnd; /* Window for capturing mouse and keyboard input */
	#endif
	boolean running; /* Note that the only procedure that is allowed to touch this value are InitServData and the update function for the child window of appWnd that represents the "Quit" button */
} ServData;


#endif
