#include "Wnd.h"


void CreateWindowHelper( Display *disp, Wnd *inParent, const windata *inDat, Wnd *outWnd )
{
	XSetWindowAttributes xswa;
	Window parent;

	xswa.background_pixel = WhitePixel( disp, DefaultScreen( disp ) );
	xswa.border_pixel = BlackPixel( disp, DefaultScreen( disp ) );
	xswa.event_mask = inDat->event_mask;
	xswa.override_redirect = inDat->override_redirect;

	if( inParent == NULL )
	{
		parent = DefaultRootWindow( disp );
	}
	else
	{
		parent = inParent->wndHandle;
	}

	outWnd->wndHandle = XCreateWindow( disp, parent, inDat->x, inDat->y, inDat->width, inDat->height, inDat->border_width, inDat->depth, inDat->class, CopyFromParent, inDat->value_mask, &xswa );

	outWnd->parent = inParent;
	outWnd->visible = FALSE;
	outWnd->data = *inDat;
	outWnd->subWnds = NULL;
	outWnd->numSubWnds = 0;
	outWnd->wndData = NULL;
}


