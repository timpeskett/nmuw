#include "NumWindow.h"

static const windata num_wnd_data = { 200, 100, 150, 100, 0, ExposureMask | StructureNotifyMask, CWOverrideRedirect | CWBorderPixel | CWBackPixel | CWEventMask, InputOutput, CopyFromParent, True, NULL, NULL, NULL }; 

static NumWndDat *CreateNumWndDat( void );
static void DestroyNumWndDat( NumWndDat *inData );


void CreateNumWnd( AppData *ad, Wnd *outWnd )
{
	CreateWindowHelper( ad->disp, NULL, &num_wnd_data, outWnd );

	outWnd->wndGC = CreateDefaultGC( ad->disp, outWnd, 0xFF0000, 0x00FF00 );
	outWnd->wndData = CreateNumWndDat();
}


NumWndDat *CreateNumWndDat( void )
{
	NumWndDat *nwd;

	nwd = malloc( sizeof( *nwd ) );

	nwd->numWnd = 0; /* Initialise to zero. it can be set later through setnumwndnumber */

	return nwd;
}

void SetNumWndNumber( Wnd *numWnd, int inNum )
{
	( (NumWndDat*)numWnd->wndData)->numWnd = inNum;
}

#ifdef __linux
void ShowNumWnd( AppData *ad, Wnd *numWnd )
{
	if( numWnd->visible == FALSE )
	{
		XMapWindow( ad->disp, numWnd->wndHandle );
	}
	numWnd->visible = TRUE;
}
#endif


#ifdef __linux
void UpdateNumWnd( AppData *ad, Wnd *numWnd )
{
	XEvent evt;
	char numString[10];
	int len;

	if( XCheckWindowEvent( ad->disp, numWnd->wndHandle, ExposureMask | StructureNotifyMask, &evt ) == True )
	{
		if( evt.type == Expose )
		{
			len = sprintf( numString, "%d", ( (NumWndDat*)numWnd->wndData)->numWnd );
			XDrawString( ad->disp, numWnd->wndHandle, numWnd->wndGC, numWnd->data.width / 2, numWnd->data.height / 2, numString, len ); 
			XFlush( ad->disp );				
		}
		if( evt.type == CreateNotify )
		{
			
		}
	}
}
#endif


#ifdef __linux
void HideNumWnd( AppData *ad, Wnd *numWnd )
{
	if( numWnd->visible == TRUE )
	{
		XUnmapWindow( ad->disp, numWnd->wndHandle );
	}
	numWnd->visible = FALSE;
}
#endif


void DestroyNumWndDat( NumWndDat *inData )
{
	free( inData );
}

void DestroyNumWnd( AppData *ad, Wnd *inWnd )
{
	DestroyNumWndDat( inWnd->wndData );
	XFreeGC( ad->disp, inWnd->wndGC );
	XDestroyWindow( ad->disp, inWnd->wndHandle );
}
