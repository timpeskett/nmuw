#ifndef APPWND_H
#define APPWND_H

#include "../common.h"
#include "../Wnd.h"
#include "../wndutil.h"
#include "CaptWnd.h"
#include "Monitor.h"
#include "ServData.h"
#include <stdio.h>
#ifdef __linux
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif


#define MAX(x,y) ((x)<(y)?(y):(x))
typedef struct
{
	int selected; /* The selected monitor. MAX_MON for none */
	boolean buttonPressed;
	double pressx, pressy; /* Percentage inside monitor where pointer pressed */
	double threshold; /* How close two monitors should be before they are snapped together */
	int leftBox, topBox, widthBox, heightBox; /* Dimensions of window 'box'. Initialised to percentages of window dimensions */
	double margin;

	int highlighted; /* The monitor that is highlighted. There is always one monitor highlighted */
	int leftList, topList, widthList, heightList;
} AppWndDat;


void CreateAppWnd( ServData *sd, Wnd *outWnd );
void ShowAppWnd( ServData *sd, Wnd *appWnd );
void UpdateAppWnd( ServData *sd, Wnd *appWnd );
void HideAppWnd( ServData *sd, Wnd *appWnd );
void DestroyAppWnd( ServData *sd, Wnd *inWnd );

void ToggleAppWnd( ServData *sd, Wnd *appWnd ); /* 'Toggles' the window for when the user presses ctrl+alt+q. Nothing to do with the toggle subwindow of the app window */

#endif

