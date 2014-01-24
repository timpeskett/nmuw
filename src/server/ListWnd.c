#include "ListWnd.h"


#ifdef __linux
static const windata list_wnd_data = { 25, 25, 50, 50, 1, ExposureMask | StructureNotifyMask | KeyPressMask | ButtonPressMask, CWOverrideRedirect | CWBorderPixel | CWBackPixel | CWEventMask, InputOutput, CopyFromParent, False, NULL, NULL, NULL };


extern const ChildCreate listWndChildren[];
extern const int listWndChildNum;
#endif


#ifdef __linux
static ListWndDat *CreateListWndDat( void  );
static void DestroyListWndDat( ListWndDat *inData );

static void AddAddress( ListWndDat *lwd, struct sockaddr_in *inAddr );
static void DrawIPList( ServData *sd, Wnd *inWnd );
static int GetEntryFromPosition( ServData *sd, Wnd *inWnd, ListWndDat *lwd, int x, int y );
#endif


void CreateListWnd( ServData *sd, Wnd *outWnd )
{
	int i;
	int screenw, screenh;

	outWnd->data = list_wnd_data;

	getMonitorDimensions( sd->mons, MAX_MON, MASTER, &screenw, &screenh );
	percentToAbsoluteDim( outWnd, screenw, screenh );
	CreateWindowHelper( sd->disp, NULL, &outWnd->data, outWnd );

	outWnd->wndGC = CreateDefaultGC( sd->disp, outWnd, 0xFF0000, 0x00FF00 );

	outWnd->numSubWnds = listWndChildNum;
	outWnd->subWnds = malloc( outWnd->numSubWnds * sizeof( *outWnd->subWnds ) );
	for( i = 0; i < outWnd->numSubWnds; i++ )
	{
		listWndChildren[i]( sd->disp, outWnd, outWnd->subWnds + i );
	}

	outWnd->wndData = CreateListWndDat();
}

#ifdef __linux
ListWndDat *CreateListWndDat( void  )
{
	ListWndDat *inData;

	inData = malloc( sizeof( *inData ) );

	inData->seperator = 2;
	inData->xmarg = 20;
	inData->ymarg = 20;

	inData->selected = 0;
	inData->addrs = NULL;
	inData->numEntries = 0;
	if( ( inData->recvSock = createListenSocket( AF_INET, SOCK_DGRAM, PORT_NUM + 1 ) ) != -1 )
	{
		if( setSocketBlock( inData->recvSock ) == -1 )
		{
			close( inData->recvSock );
			free( inData );
			inData = NULL;
		}
	}
	else
	{
		free( inData );
		inData = NULL;
	}

	return inData;
}
#endif


#ifdef __linux
void DestroyListWndDat( ListWndDat *inDat )
{
	if( inDat != NULL )
	{
		free( inDat->addrs );
		close( inDat->recvSock );
	}
	free( inDat );
}
#endif

#ifdef __linux
void ShowListWnd( ServData *sd, Wnd *listWnd )
{
	int i;

	if( listWnd->visible == FALSE )
	{
		XMapSubwindows( sd->disp, listWnd->wndHandle );
		XMapWindow( sd->disp, listWnd->wndHandle );
		if( listWnd->wndData == NULL )
		{
			listWnd->wndData = CreateListWndDat();
		}
	}
	listWnd->visible = TRUE;

	for( i = 0; i < listWnd->numSubWnds; i++ )
	{
		listWnd->subWnds[i].visible = TRUE;
	}
}
#endif


#ifdef __linux
void AddAddress( ListWndDat *lwd, struct sockaddr_in *inAddr )
{
	struct sockaddr_in *newArr;
	boolean present = FALSE;
	int i;

	
	if( inAddr != NULL )
	{
		for( i = 0; i < lwd->numEntries; i++ )
		{
			if( lwd->addrs[i].sin_addr.s_addr == inAddr->sin_addr.s_addr )
			{
				present = TRUE;
			}
		}

		if( !present )
		{
			newArr = malloc( ( lwd->numEntries + 1 ) * sizeof( struct sockaddr_in ) );
			for( i = 0; i < lwd->numEntries; i++ )
			{
				newArr[i] = lwd->addrs[i];
			}
			newArr[ lwd->numEntries ] = *inAddr;
			lwd->numEntries++;
			free( lwd->addrs );
			lwd->addrs = newArr;
		}
	}
}
#endif

#ifdef __linux
void UpdateListWnd( ServData *sd, Wnd *listWnd )
{
	XEvent evt;
	int i;
	Bool found;

	if( listWnd->visible == TRUE  )
	{	
		struct sockaddr_in inAddr;

		if( recvBroadcast( ((ListWndDat*)listWnd->wndData)->recvSock, &inAddr ) == 0 )
		{
			AddAddress( (ListWndDat*)listWnd->wndData, &inAddr );
			DrawIPList( sd, &sd->listWnd );
		}

		if( XEventsQueued( sd->disp, QueuedAlready ) != 0 )
		{
			if( XCheckWindowEvent( sd->disp, listWnd->wndHandle, listWnd->data.event_mask, &evt ) == True )
			{
				switch( evt.type )
				{
					case Expose:
						DrawIPList( sd, &sd->listWnd );
						break;
					case ButtonPress:
						( ( ListWndDat* )listWnd->wndData)->selected = GetEntryFromPosition( sd, &sd->listWnd, ( ListWndDat* )listWnd->wndData, evt.xbutton.x, evt.xbutton.y );
						DrawIPList( sd, &sd->listWnd );
						break;
					default:
						break;
				}
			}
			else
			{
				i = 0;
				found = False;
				while( !found && i < listWnd->numSubWnds )
				{
					if( XCheckWindowEvent( sd->disp, listWnd->subWnds[i].wndHandle, listWnd->subWnds[i].data.event_mask, &evt ) == True )
					{
						if( listWnd->subWnds[i].data.updatewnd != NULL )
						{
							listWnd->subWnds[i].data.updatewnd( sd, &evt, &listWnd->subWnds[i] );
						}
					}
					i++;
				}
			}
		}
	}
	else
	{
		XFlush( sd->disp );
	}
}
#endif


#ifdef __linux
void DrawIPList( ServData *sd, Wnd *inWnd )
{
	int i;
	XFontStruct *gcFont;
	int width, ascent, descent, direction;
	XCharStruct overall_return;
	char ipString[INET_ADDRSTRLEN + 1];
	ListWndDat *wndDat;
	const char *text = "Available Clients:";

	wndDat = (ListWndDat*)inWnd->wndData;

	gcFont = XQueryFont( sd->disp, XGContextFromGC( inWnd->wndGC ) );
	XTextExtents( gcFont, text, strlen( text ), &direction, &ascent, &descent, &overall_return );

	width = XTextWidth( gcFont, text, strlen( text ) );

	if( width < inWnd->data.width )
	{
		XDrawImageString( sd->disp, inWnd->wndHandle, inWnd->wndGC, wndDat->xmarg, wndDat->ymarg + ascent, text, strlen( text ) );
	}

	i = 0;
	while( i < wndDat->numEntries && i * ( ascent + descent + wndDat->seperator ) < inWnd->data.height )
	{
		inet_ntop( AF_INET, &wndDat->addrs[i].sin_addr, ipString, sizeof( ipString ) ); 
		width = XTextWidth( gcFont, ipString, strlen( ipString ) );

		if( width < inWnd->data.width )
		{
			/* Make the selected entry stand out by highlighting it */
			if( i == wndDat->selected - 1 )
			{
				XSetForeground( sd->disp, inWnd->wndGC, WhitePixel( sd->disp, DefaultScreen( sd->disp ) ) );
				XSetBackground( sd->disp, inWnd->wndGC, BlackPixel( sd->disp, DefaultScreen( sd->disp ) ) );
			}

			XDrawImageString( sd->disp, inWnd->wndHandle, inWnd->wndGC, wndDat->xmarg, ( i + 1 ) * ( ascent + descent + wndDat->seperator ) + ascent + wndDat->ymarg, ipString, strlen( ipString ) );
			
			/* Drawing done, change colour back */
			if( i == wndDat->selected - 1 )
			{
				XSetForeground( sd->disp, inWnd->wndGC, BlackPixel( sd->disp, DefaultScreen( sd->disp ) ) );
				XSetBackground( sd->disp, inWnd->wndGC, WhitePixel( sd->disp, DefaultScreen( sd->disp ) ) );
			}
		}
		i++;
	}

	XFreeFontInfo( NULL, gcFont, 0 );
}
#endif


#ifdef __linux
int GetEntryFromPosition( ServData *sd, Wnd *inWnd, ListWndDat *lwd, int x, int y )
{
	XFontStruct *gcFont;
	int direction, ascent, descent;
	XCharStruct overall_return;
	int entry;
	int entryheight;
	const char *text = "Test";

	gcFont = XQueryFont( sd->disp, XGContextFromGC( inWnd->wndGC ) );
	XTextExtents( gcFont, text, strlen( text ), &direction, &ascent, &descent, &overall_return );
	XFreeFontInfo( NULL, gcFont, 0 );

	entryheight = ascent + descent + lwd->seperator;
	entry = ( y - lwd->ymarg ) / entryheight;
	if( ! ( 1 <= entry && entry <= lwd->numEntries ) )
	{
		entry = -1;
	}

	return entry;
}	
#endif


#ifdef __linux
void HideListWnd( ServData *sd, Wnd *listWnd )
{
	int i;

	if( listWnd->visible == TRUE )
	{
		XUnmapWindow( sd->disp, listWnd->wndHandle );
		XUnmapSubwindows( sd->disp, listWnd->wndHandle );
		
	}
	listWnd->visible = FALSE;

	for( i = 0; i < listWnd->numSubWnds; i++ )
	{
		listWnd->visible = FALSE;
	}
}
#endif

void DestroyListWnd( ServData *sd, Wnd *inWnd )
{
	int i;

	for( i = 0; i < inWnd->numSubWnds; i++ )
	{
		/* Call the destroy function of each child window */
		inWnd->subWnds[i].data.destroywnd( sd->disp, inWnd->subWnds + i );
	}
	DestroyListWndDat( inWnd->wndData );
	XDestroySubwindows( sd->disp, inWnd->wndHandle );
	free( inWnd->subWnds );

	XFreeGC( sd->disp, inWnd->wndGC );
	XDestroyWindow( sd->disp, inWnd->wndHandle );
}
