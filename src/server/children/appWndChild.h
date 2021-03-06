#ifndef APPWNDCHILD_H
#define APPWNDCHILD_H

#include "../../common.h"
#include "../../Wnd.h"
#include "../../netmsg.h"
#include "../ServData.h"
#include "../../wndutil.h"
#include "../ListWnd.h"
#include <X11/Xlib.h>

/* "Connect" Button in the app window */
void appWndConnectCreate( Display *disp, Wnd *parent, Wnd *outWnd );
void appWndConnectUpdate( ServData *sd, XEvent *evt, Wnd *subWnd );
void appWndConnectDestroy( Display *disp, Wnd *inWnd );

/* "Toggle Numbering" button in the app window */
void appWndToggleCreate( Display *disp, Wnd *parent, Wnd *outWnd );
void appWndToggleUpdate( ServData *sd, XEvent *evt, Wnd *subWnd );
void appWndToggleDestroy( Display *disp, Wnd *inWnd );

/* "Quit" Button in the app window */
void appWndQuitCreate( Display *disp, Wnd *parent, Wnd *outWnd );
void appWndQuitUpdate( ServData *sd, XEvent *evt, Wnd *subWnd );
void appWndQuitDestroy( Display *disp, Wnd *inWnd );


#endif



