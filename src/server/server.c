#include "server.h"

static boolean InitServData( ServData *sd );
static void DestroyServData( ServData *sd );

/* Grabs Alt+Q so it is always sent to appwnd */
static void SetGlobalHotkey( ServData *sd );
static void RemoveGlobalHotkey( ServData *sd );



int main( void )
{
	ServData sd;

	if( InitServData( &sd ) == FALSE )
	{
		fprintf( stderr, "Could not initialise server data!\n" );
		return 1;
	}

	SetGlobalHotkey( &sd );

	while( sd.running ) /*Member variable accessed here with great caution */
	{
		UpdateAppWnd( &sd, &sd.appWnd );
		UpdateListWnd( &sd, &sd.listWnd );
		UpdateCaptWnd( &sd, &sd.captWnd );
		#ifdef __linux
		usleep( SLEEP_TIME );
		#endif
	}

	RemoveGlobalHotkey( &sd );

	DestroyServData( &sd );
	
	return 0;
}


boolean InitServData( ServData *sd )
{
	int dispWidth, dispHeight;

	if( ( sd->disp = XOpenDisplay( NULL ) ) == NULL )
	{
		return FALSE;
	}

	sd->running = TRUE;

	memset( sd->mons, 0, sizeof( sd->mons ) );
	
	dispWidth = DisplayWidth( sd->disp, DefaultScreen( sd->disp ) );
	dispHeight = DisplayHeight( sd->disp, DefaultScreen( sd->disp ) );
	/* Initialise Master Monitor */
	sd->focusMon = sd->curMon = MASTER;
	sd->lastFocus = MAX_MON;
	sd->mons[MASTER] = createMonitor( NULL, 0 );
	setMonitorDimensions( sd->mons, MAX_MON, MASTER, dispWidth, dispHeight, 0.0 );
	/* threshold does not matter here as master monitor is guaranteed to be the only monitor active at this point */

	/* Create Main application window */
	CreateAppWnd( sd, &sd->appWnd );
	/* Create List Window */
	CreateListWnd( sd, &sd->listWnd );

	/* Create special capture window for capturing mouse movements and keystrokes */
	CreateCaptWnd( sd, &sd->captWnd );

	return TRUE;
}


#ifdef __linux
void DestroyServData( ServData *sd )
{
	int i;

	for( i = 0; i < MAX_MON; i++ )
	{
		if( sd->mons[i] != NULL )
		{
			destroyMonitor( sd->mons[i] );
			sd->mons[i] = NULL;
		}
	}
	
	DestroyAppWnd( sd, &sd->appWnd ); 
	DestroyListWnd( sd, &sd->listWnd );

	DestroyCaptWnd( sd, &sd->captWnd );

	XCloseDisplay( sd->disp );
}
#endif


#ifdef __linux
void SetGlobalHotkey( ServData *sd )
{
	XGrabKey( sd->disp, XKeysymToKeycode( sd->disp, XK_Q ), Mod1Mask | ControlMask, DefaultRootWindow( sd->disp ), False, GrabModeAsync, GrabModeAsync ); 
}
#endif

#ifdef __linux
void RemoveGlobalHotkey( ServData *sd )
{
	XUngrabKey( sd->disp, XKeysymToKeycode( sd->disp, XK_Q ), Mod1Mask, DefaultRootWindow( sd->disp ) );
}
#endif

