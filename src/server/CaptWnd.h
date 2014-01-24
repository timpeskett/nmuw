#ifndef CAPTWND_H
#define CAPTWND_H

#include "../common.h"
#include "../Wnd.h"
#include "Monitor.h"
#include "ServData.h"
#include "../msgs.h"
#include "../netmsg.h"
#include "../fakeinput.h"
#include "AppWnd.h"

#ifdef __linux
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#endif

typedef struct
{
	boolean keyboardCapt, pointerCapt;
	int lastx, lasty; /* Coordinates of point on screen that pointer was last at */
	int margin; /* Margin around screen which if the pointer moves past it will be moved back to the middle */
	int threshold; /* Margin around screen which if pointer moves past then it will move to next monitor. only applies to master monitor */
	Cursor transCurs;
} CaptWndDat;

void CreateCaptWnd( ServData *sd, Wnd *outWnd );
void ShowCaptWnd( ServData *sd, Wnd *captWnd );
void UpdateCaptWnd( ServData *sd, Wnd *captWnd );
void HideCaptWnd( ServData *sd, Wnd *captWnd );
void DestroyCaptWnd( ServData *sd, Wnd *inWnd );

void CaptKeyboard( ServData *sd, Wnd *captWnd );
void UncaptKeyboard( ServData *sd, Wnd *captWnd );
void CaptPointer( ServData *sd, Wnd *captWnd );
void UncaptPointer( ServData *sd, Wnd *captWnd );



#endif
