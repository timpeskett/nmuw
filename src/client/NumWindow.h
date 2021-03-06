#ifndef NUMWINDOW_H
#define NUMWINDOW_H

#include "../common.h"
#include "AppData.h"
#include <stdio.h>
#include <stdlib.h>
#include "../wndutil.h"
#ifdef __linux
#include <X11/Xlib.h>
#endif


typedef struct
{
	int numWnd; /* The number of the window */
} NumWndDat;

void CreateNumWnd( AppData *ad, Wnd *outWnd );
void ShowNumWnd( AppData *ad, Wnd *numWnd );
void UpdateNumWnd( AppData *ad, Wnd *numWnd );
void HideNumWnd( AppData *ad, Wnd *numWnd );
void DestroyNumWnd( AppData *ad, Wnd *inWnd );

void SetNumWndNumber( Wnd *numWnd, int inNum );

#endif
