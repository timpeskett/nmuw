#ifndef LISTWNDCHILD_H
#define LISTWNDCHILD_H


#include "../../common.h"
#include "../../Wnd.h"
#include "../../wndutil.h"
#include "../ServData.h"
#include "../ListWnd.h"
#include <X11/Xlib.h>

/* LIST WINDOW CHILDREN */

/* "Connect button in list window */
void listWndConnectCreate( Display *disp, Wnd *parent, Wnd *outWnd );
void listWndConnectUpdate( ServData *sd, XEvent *evt, Wnd *subWnd );
void listWndConnectDestroy( Display *disp, Wnd *inWnd );


/* "Cancel" button in list window */
void listWndCancelCreate( Display *disp, Wnd *parent, Wnd *outWnd );
void listWndCancelUpdate( ServData *sd, XEvent *evt, Wnd *subWnd );
void listWndCancelDestroy( Display *disp, Wnd *inWnd );

#endif
