#include "appWndChild.h"

/* This is required so the parent knows how many subwindows are required and how to create them. After creation, the update and destroy functions can be accessed through the subwindow's data */
const ChildCreate appWndChildren[] = { appWndConnectCreate, appWndToggleCreate, appWndQuitCreate };
const int appWndChildNum = sizeof( appWndChildren ) / sizeof( *appWndChildren );

/* Note that here all size values ( except for border_value ) are actually percentages [0,100] for where the child should lay on the screen. 80 for x means that the child's x position should be 80% of the parent's width. These need to be converted to actual values in the appWndxxxxxCreate function */
static const windata app_wnd_connect = { 20, 4, 15, 8, 1, ExposureMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask, CWOverrideRedirect | CWBorderPixel | CWBackPixel | CWEventMask, InputOutput, CopyFromParent, False, appWndConnectCreate, (ChildUpdate)appWndConnectUpdate, appWndConnectDestroy };

static const windata app_wnd_toggle = { 40, 4, 15, 8, 1, ExposureMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask, CWOverrideRedirect | CWBorderPixel | CWBackPixel | CWEventMask, InputOutput, CopyFromParent, False, appWndToggleCreate, (ChildUpdate)appWndToggleUpdate, appWndToggleDestroy }; 

static const windata app_wnd_quit = { 60, 4, 15, 8, 1, ExposureMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask, CWOverrideRedirect | CWBorderPixel | CWBackPixel | CWEventMask, InputOutput, CopyFromParent, False, appWndQuitCreate, (ChildUpdate)appWndQuitUpdate, appWndQuitDestroy }; 
	

/*****************************************************8
 * 		CONNECT BUTTON
 */
void appWndConnectCreate( Display *disp, Wnd *parent, Wnd *outWnd )
{
	outWnd->data = app_wnd_connect;

	percentToAbsoluteDim( outWnd, parent->data.width, parent->data.height );
	CreateWindowHelper( disp, parent, &outWnd->data, outWnd );
	outWnd->wndGC = parent->wndGC; 
}

void appWndConnectUpdate( ServData *sd, XEvent *evt, Wnd *subWnd )
{
	const char label[] = "Connect";

	switch( evt->type )
	{
		case Expose:
			drawCenteredText( sd->disp, subWnd, label, sizeof( label ) - 1 );
			break;
		case ButtonRelease:
			/* One of the few places where information that
			 * shouldn't be accessed is. Would have to implement a
			 * sibling function to do this properly but I'm not
			 * sure if that is worth the effort */
			ShowListWnd( sd, &sd->listWnd );
			break;
		default:
			break;
	}
}

void appWndConnectDestroy( Display *disp, Wnd *inWnd )
{
	/* GC belongs to parent, we just borrow it. no need for us to free it */
	XDestroyWindow( disp, inWnd->wndHandle );
}

/***********************************************
 * 			TOGGLE BUTTON
 */

typedef struct
{
	boolean isToggle;
} ToggleWndDat;

static ToggleWndDat *CreateToggleWndDat( void )
{
	ToggleWndDat *twd;

	twd = malloc( sizeof( twd ) );
	twd->isToggle = FALSE;

	return twd;
}

static void DestroyToggleWndDat( ToggleWndDat *twd )
{
	free( twd );
}

void appWndToggleCreate( Display *disp, Wnd *parent, Wnd *outWnd )
{
	outWnd->data = app_wnd_toggle;

	percentToAbsoluteDim( outWnd, parent->data.width, parent->data.height );
	CreateWindowHelper( disp, parent, &outWnd->data, outWnd );
	outWnd->wndGC = parent->wndGC;

	outWnd->wndData = CreateToggleWndDat();
}

void appWndToggleUpdate( ServData *sd, XEvent *evt, Wnd *subWnd )
{
	const char label[] = "Toggle";
	ToggleWndDat *wndDat;

	wndDat = subWnd->wndData;

	switch( evt->type )
	{
		case Expose:
			drawCenteredText( sd->disp, subWnd, label, sizeof( label ) - 1 );
			break;
		case ButtonRelease:
			{
				int i;

				wndDat->isToggle = !wndDat->isToggle;
				for( i = 0; i < MAX_MON; i++ )
				{
					if( i != MASTER && sd->mons[i] != NULL )
					{
						displayMonitorNumber( sd->mons[i], i, wndDat->isToggle );
					}
				}		
			}
			break;
		default:
			break;
	}
}

void appWndToggleDestroy( Display *disp, Wnd *inWnd )
{
	DestroyToggleWndDat( inWnd->wndData );
	/* GC belongs to parent, we just borrow it. no need for us to free it */
	XDestroyWindow( disp, inWnd->wndHandle );
}


/*************************************************
 * 			QUIT BUTTON
 */
void appWndQuitCreate( Display *disp, Wnd *parent, Wnd *outWnd )
{
	outWnd->data = app_wnd_quit;

	percentToAbsoluteDim( outWnd, parent->data.width, parent->data.height );
	CreateWindowHelper( disp, parent, &outWnd->data, outWnd );
	outWnd->wndGC = parent->wndGC;
}

void appWndQuitUpdate( ServData *sd, XEvent *evt, Wnd *subWnd )
{
	const char label[] = "Quit";
	switch( evt->type )
	{
		case Expose:
			drawCenteredText( sd->disp, subWnd, label, sizeof( label ) - 1 );
			break;
		case ButtonRelease:
			/* This information is meant to be only accessed by
			 * this function but I still don't really like it */
			sd->running = FALSE;
			break;
		default:
			break;
	}
}

void appWndQuitDestroy( Display *disp, Wnd *inWnd )
{
	/* GC belongs to parent, we just borrow it. no need for us to free it */
	XDestroyWindow( disp, inWnd->wndHandle );
}


