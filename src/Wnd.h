#ifndef WND_H
#define WND_H

#include "common.h"
#ifdef __linux
#include <X11/Xlib.h>
#endif

/* Forward declare this structure otherwise we have a circular dependency in this file */
struct Wnd;

/* These are function pointer types for the create, update and destroy functions of child windows so that they can be accessed 'anonymously' */
typedef void (*ChildCreate)( Display *, struct Wnd *, struct Wnd * );
typedef void (*ChildUpdate)( void *, XEvent *, struct Wnd * ); /* void pointer here takes either AppData or ServData */
typedef void (*ChildDestroy)( Display *, struct Wnd * );


/* Mainly used to create const global data used for window creation */
typedef struct _windata
{
	int x, y, width, height;
	int border_width;
#ifdef __linux
	long event_mask;
	long value_mask;
	unsigned int class;
	int depth;
        Bool override_redirect;
	ChildCreate createwnd;
	ChildUpdate updatewnd;
	ChildDestroy destroywnd;
#endif
} windata;


typedef struct Wnd
{
	windata data;
	boolean visible;
#ifdef __linux
	Window wndHandle;
	struct Wnd *parent;
	struct Wnd *subWnds;
	int numSubWnds;
	GC wndGC;
	void *wndData; /*Window specific data, put whatever you want here and it will persist through calls to UpdateXXXXWindow() */
#endif
} Wnd;



void CreateWindowHelper( Display *disp, Wnd *inParent, const windata *inDat, Wnd *outWnd );

#endif
