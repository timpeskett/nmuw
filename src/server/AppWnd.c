#include "AppWnd.h"

#ifdef __linux
/* Dimensions in percentages of screen size */
static const windata app_wnd_data = { 25, 25, 50, 50, 1, ExposureMask | StructureNotifyMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask, CWOverrideRedirect | CWBorderPixel | CWBackPixel | CWEventMask, InputOutput, CopyFromParent, True, NULL, NULL, NULL };

extern const ChildCreate appWndChildren[];
extern const int appWndChildNum;
#endif


#ifdef __linux

static AppWndDat *CreateAppWndDat( void  );
static void DestroyAppWndDat( AppWndDat *inData );

static void DrawMonitorRect( ServData *sd, Wnd *appWnd, int num, int x, int y, int width, int height );
static void DrawAppWndBox( ServData *sd, Wnd *appWnd, boolean clear );
static void BoxToMonitorRect( ServData *sd, Wnd *appWnd, int *x, int *y, int *width, int *height );
static void MonitorToBoxRect( ServData *sd, Wnd *appWnd, int *x, int *y, int *width, int *height );

static int *ListMonitors( ServData *sd );
static void DrawMonitorList( ServData *sd, Wnd *appWnd );

#endif


void CreateAppWnd( ServData *sd, Wnd *outWnd )
{
	int i;
	int screenw, screenh;
	
	outWnd->data = app_wnd_data;
	
	getMonitorDimensions( sd->mons, MAX_MON, MASTER, &screenw, &screenh );
	percentToAbsoluteDim( outWnd, screenw, screenh ); 
	CreateWindowHelper( sd->disp, NULL, &outWnd->data, outWnd );

	outWnd->wndGC = CreateDefaultGC( sd->disp, outWnd, 0xFF0000, 0x00FF00 );

	outWnd->numSubWnds = appWndChildNum;
	outWnd->subWnds = malloc( outWnd->numSubWnds * sizeof( *outWnd->subWnds ) );
	for( i = 0; i < outWnd->numSubWnds; i++ )
	{
		appWndChildren[i]( sd->disp, outWnd, outWnd->subWnds + i );
	}	

	outWnd->wndData = CreateAppWndDat();
}

#ifdef __linux
AppWndDat *CreateAppWndDat( void )
{
	AppWndDat *awd;

	awd = malloc( sizeof( *awd ) );
	
	awd->selected = MAX_MON;
	awd->buttonPressed = FALSE;
	awd->threshold = 0.1;
	awd->leftBox = 10;
	awd->topBox = 15;
	awd->widthBox = 80;
	awd->heightBox = 50;
	awd->margin = 1.25;

	awd->leftList = 0;
	awd->topList = 70;
	awd->widthList = 100;
	awd->heightList = 30;
	awd->highlighted = MASTER;

	return awd;
}
#endif


#ifdef __linux
void DestroyAppWndDat( AppWndDat *awd )
{
	free( awd );
}
#endif

#ifdef __linux
void ShowAppWnd( ServData *sd, Wnd *appWnd )
{
	int i;
	if( appWnd->visible == FALSE )
	{
		XMapSubwindows( sd->disp, appWnd->wndHandle );
		XMapWindow( sd->disp, appWnd->wndHandle );
	}
	appWnd->visible = TRUE;
	for( i = 0; i < appWnd->numSubWnds; i++ )
	{
		appWnd->subWnds[i].visible = TRUE;
	}
	( (AppWndDat*)appWnd->wndData)->highlighted = sd->focusMon;
}
#endif

/* Processes one message per call */
#ifdef __linux
void UpdateAppWnd( ServData *sd, Wnd *appWnd )
{
	XEvent evt;
	int i;
	Bool found;
	int monNum;
	AppWndDat *awd;

	awd = appWnd->wndData;

	/* We need to get messages for this window even when it is not visible */
	if( XEventsQueued( sd->disp, QueuedAlready ) != 0 )
	{
		if( XCheckWindowEvent( sd->disp, appWnd->wndHandle, appWnd->data.event_mask, &evt ) == True )
		{
			switch( evt.type )
			{
				case Expose:
					DrawAppWndBox( sd, &sd->appWnd, TRUE ); 
					DrawMonitorList( sd, &sd->appWnd );
					break;
				case KeyPress:
					{
						int *monList;
						int numMons;
						int i;

						monList = ListMonitors( sd );
						numMons = countActiveMonitors( sd->mons, MAX_MON );
						while( awd->highlighted != monList[i] )
						{
							i++;
						}

						switch( XLookupKeysym( &evt.xkey, 0 ) )
						{
							case XK_q:
								if( evt.xkey.state & Mod1Mask && evt.xkey.state & ControlMask )
								{
									printf("We have liftoff!!!\n" );
								}
								break;
							case XK_w:
								if( i > 0 )
								{
									awd->highlighted = monList[ i - 1 ];
								}
								DrawMonitorList( sd, &sd->appWnd );
								break;
							case XK_s:
								if( i < numMons - 1 )
								{
									awd->highlighted = monList[ i + 1 ];
								}
								DrawMonitorList( sd, &sd->appWnd );
								break;
							default:
								break;
						}
					}									
					break;
				case ButtonPress:
					{
						int monx, mony, monw, monh;

						monx = evt.xbutton.x;
						mony = evt.xbutton.y;
						monw = monh = 1; /*Dummy values, not needed*/
						BoxToMonitorRect( sd, &sd->appWnd, &monx, &mony, &monw, &monh );

						if( ( monNum = getMonitorFromPoint( sd->mons, MAX_MON, monx, mony ) ) != -1 )
						{
							int left, top, width, height;
							getMonitorPosition( sd->mons, MAX_MON, monNum, &left, &top, &width, &height );
							width -= left;
							height -= top;

						        awd->pressx = ( monx - left ) / (double)width;
							awd->pressy = ( mony - top ) / (double)height;	
							awd->selected = monNum;
							awd->buttonPressed = TRUE;
						}
					}
					break;
				case MotionNotify:
					if( awd->buttonPressed == TRUE )
					{
						int monx, mony, monw, monh;
						int left, top, width, height;

						monx = evt.xmotion.x;
						mony = evt.xmotion.y;
						monw = monh = 1;

						getMonitorPosition( sd->mons, MAX_MON, awd->selected, &left, &top, &width, &height );
						width -= left;
						height -= top;

						BoxToMonitorRect( sd, &sd->appWnd, &monx, &mony, &monw, &monh );
						left = monx - awd->pressx * width;
						top = mony - awd->pressy * height;
						setMonitorPosition( sd->mons, MAX_MON, awd->selected, left, top, awd->threshold );
						DrawAppWndBox( sd, &sd->appWnd, TRUE );
					}
					break;	
				case ButtonRelease:
					if( awd->buttonPressed == TRUE )
					{
						int monx, mony, monw, monh;
						int left, top, width, height;

						monx = evt.xbutton.x;
						mony = evt.xbutton.y;
						monw = monh = 1; /*Dummy values */

						getMonitorPosition( sd->mons, MAX_MON, awd->selected, &left, &top, &width, &height );
						width -= left;
						height -= top;

						BoxToMonitorRect( sd, &sd->appWnd, &monx, &mony, &monw, &monh );
						left = monx - awd->pressx * width;
						top = mony - awd->pressy * height;
						setMonitorPosition( sd->mons, MAX_MON, awd->selected, left, top, awd->threshold );


						awd->buttonPressed = FALSE;
						DrawAppWndBox( sd, &sd->appWnd, TRUE );
					}
					break;
				default:
					break; 
			}
		}
		else if( XCheckWindowEvent( sd->disp, DefaultRootWindow( sd->disp ), KeyPressMask,  &evt ) == True )
		{
			switch( evt.type )
			{
				case KeyPress:
					if( evt.xkey.keycode == XKeysymToKeycode( sd->disp, XK_Q ) && evt.xkey.state & Mod1Mask && evt.xkey.state & ControlMask  )
					{
						ToggleAppWnd( sd, appWnd );	
					}
					break;
				default:
					break;
			}
		}
		else
		{
			i = 0;
			found = False;
			while( !found && i < appWnd->numSubWnds )
			{
				if( XCheckWindowEvent( sd->disp, appWnd->subWnds[i].wndHandle, appWnd->subWnds[i].data.event_mask, &evt ) == True )
				{
					if( appWnd->subWnds[i].data.updatewnd != NULL )
					{
						appWnd->subWnds[i].data.updatewnd( sd, &evt, &appWnd->subWnds[i] );
					}
				}
				i++;
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
void BoxToMonitorRect( ServData *sd, Wnd *appWnd, int *x, int *y, int *width, int *height )
{
	int boundx, boundy, boundw, boundh;
	double multiplier;
	int boxx, boxy, boxw, boxh;
	int midboundx, midboundy;
	AppWndDat *wndData;

	wndData = (AppWndDat*)appWnd->wndData;

	getBoundingRectangle( sd->mons, MAX_MON, &boundx, &boundy, &boundw, &boundh );
	boundw -= boundx;
	boundh -= boundy;

	boxx = ( wndData->leftBox / 100.0 ) * appWnd->data.width;
	boxy = ( wndData->topBox / 100.0 ) * appWnd->data.height;
	boxw = ( wndData->widthBox / 100.0 ) * appWnd->data.width;
	boxh = ( wndData->heightBox / 100.0 ) * appWnd->data.height;
	
	multiplier = MIN( (double)boxw / boundw, (double)boxh / boundh );
        multiplier /= wndData->margin;

	midboundx = boundx + boundw / 2;
	midboundy = boundy + boundh / 2;

	*x =  ( ( *x - ( boxx + boxw / 2 ) ) / multiplier ) + midboundx;
	*y = ( ( *y - ( boxy + boxh / 2 ) ) / multiplier ) + midboundy;
	*width = *width / multiplier;
	*height = *height / multiplier;
}
#endif

#ifdef __linux
void MonitorToBoxRect( ServData *sd, Wnd *appWnd, int *x, int *y, int *width, int *height )
{
	int boundx, boundy, boundw, boundh;
	double multiplier;
	int midboundx, midboundy;
	int boxx, boxy, boxw, boxh;
	AppWndDat *wndData;

	wndData = (AppWndDat*)appWnd->wndData;

	getBoundingRectangle( sd->mons, MAX_MON, &boundx, &boundy, &boundw, &boundh );
	boundw -= boundx;
	boundh -= boundy;

	boxx = ( wndData->leftBox / 100.0 ) * appWnd->data.width;
	boxy = ( wndData->topBox / 100.0 ) * appWnd->data.height;
	boxw = ( wndData->widthBox / 100.0 ) * appWnd->data.width;
	boxh = ( wndData->heightBox / 100.0 ) * appWnd->data.height;
	
	multiplier = MIN( (double)boxw / boundw, (double)boxh / boundh );
        multiplier /= wndData->margin;

	midboundx = boundx + boundw / 2;
	midboundy = boundy + boundh / 2;

	*x = boxx + boxw / 2 - (midboundx - *x ) * multiplier;
	*y = boxy + boxh / 2 - ( midboundy - *y ) * multiplier;
        *width = *width * multiplier;
	*height = *height * multiplier;	
}
#endif



#ifdef __linux
void DrawAppWndBox( ServData *sd, Wnd *appWnd, boolean clear )
{
	int i;
	int x, y, width, height;
	AppWndDat *wndData;

	wndData = (AppWndDat*)appWnd->wndData;

	x = ( wndData->leftBox / 100.0 ) * appWnd->data.width;
	y = ( wndData->topBox / 100.0 ) * appWnd->data.height;
	width = ( wndData->widthBox / 100.0 ) * appWnd->data.width;
	height = ( wndData->heightBox / 100.0 ) * appWnd->data.height;

	if( clear == TRUE )
	{
		XClearArea( sd->disp, appWnd->wndHandle, x, y, width, height, False );
	}

	XDrawRectangle( sd->disp, appWnd->wndHandle, appWnd->wndGC, x, y, width, height );

	for( i = 0; i < MAX_MON; i++ )
	{
		if( sd->mons[i] != NULL )
		{
			int monx, mony, monw, monh;

			monx = sd->mons[i]->x;
			mony = sd->mons[i]->y;
			monw = sd->mons[i]->width;
			monh = sd->mons[i]->height;

			MonitorToBoxRect( sd, appWnd, &monx, &mony, &monw, &monh );

			DrawMonitorRect( sd, appWnd, i, monx, mony, monw, monh );
		}
	}
}
#endif


void DrawMonitorList( ServData *sd, Wnd *appWnd )
{
	AppWndDat *awd;
	int x, y, width, height;
	int *monList;
	int numMon, curMon;
	int textHeight;
	char outString[ sizeof( "Monitor Master" ) + 1 ];

	awd = appWnd->wndData;
	x = (int)( awd->leftList / 100.0 * appWnd->data.width );
	y = (int)( awd->topList / 100.0 * appWnd->data.height );
	width = (int)( awd->widthList / 100.0 * appWnd->data.width );
	height = (int)( awd->heightList / 100.0 * appWnd->data.height );

	monList = ListMonitors( sd ); 
		
	textHeight = GetTextHeight( sd->disp, appWnd, "Tg" );
	numMon = countActiveMonitors( sd->mons, MAX_MON );
	for( curMon = 0; curMon < numMon; curMon++ )
	{
		boolean invert = FALSE;

		if( awd->highlighted == monList[curMon] )
		{
			invert = TRUE;
		}	

		if( monList[curMon] == MASTER )
		{
			sprintf( outString, "Monitor Master" );
		}
		else
		{
			sprintf( outString, "Monitor %d", monList[curMon] );
		}
		DrawText( sd->disp , appWnd, outString, x, y + curMon * textHeight , width, height, invert );
	}

	free( monList );
}


int *ListMonitors( ServData *sd )
{
	int *monList;
	int numMon, curMon;

	monList = malloc( countActiveMonitors( sd->mons, MAX_MON ) * sizeof( *monList ) ); 

	monList[0] = sd->curMon;
	numMon = curMon = 1;
	/* Make sure that the last monitor in focus is second as that is probably what the user wants */
	if( sd->lastFocus != MAX_MON && sd->lastFocus != monList[0] )
	{
		monList[1] = sd->lastFocus;
		numMon++;
	}
	while( curMon < MAX_MON )
	{
		if( curMon != sd->lastFocus && curMon != monList[0] )
		{
			if( sd->mons[numMon] != NULL )
			{
				monList[numMon] = curMon;
				numMon++;
			}
		}
		curMon++;
	}

	return monList; 
}

#ifdef __linux
void DrawMonitorRect( ServData *sd, Wnd *appWnd, int num, int x, int y, int width, int height )
{
	XFontStruct *gcFont;
	int fontwidth;
	int midx, midy, monLen;
	char *monName;
	const char *master = "Master";

	monName = malloc( ( strlen( master ) + 1 ) * sizeof( *master ) );

	if( num == MASTER )
	{
		strcpy( monName, master );
		monLen = strlen( master );
	}
	else
	{
		monLen = sprintf( monName, "%d", num );
	}
	gcFont = XQueryFont( sd->disp, XGContextFromGC( appWnd->wndGC ) );
	fontwidth = XTextWidth( gcFont, monName, monLen );
	XFreeFontInfo( NULL, gcFont, 0 );

	midx = x + width / 2;
        midy = y + height / 2;	

	XDrawRectangle( sd->disp, appWnd->wndHandle, appWnd->wndGC, x, y, width, height );
	XDrawImageString( sd->disp, appWnd->wndHandle, appWnd->wndGC, midx - fontwidth / 2, midy, monName, monLen );
}
#endif


void ToggleAppWnd( ServData *sd, Wnd *appWnd )
{
	AppWndDat *awd;

	awd = appWnd->wndData;
	if( appWnd->visible == TRUE )
	{
		HideAppWnd( sd, appWnd );
		if( awd->highlighted != sd->focusMon )
		{
			if( sd->focusMon == MASTER )
			{
				/* Keyboard is not captured so capture it */
				CaptKeyboard( sd, &sd->captWnd );
			}
			else if( awd->highlighted == MASTER )
			{
				/* Keyboard is captured, uncapture it */
				UncaptKeyboard( sd, &sd->captWnd );
			}

			if( awd->highlighted != sd->curMon )
			{
				sd->lastFocus = sd->curMon;
			}
		}
		sd->focusMon = sd->curMon = awd->highlighted;
	}
	else
	{
		ShowAppWnd( sd, appWnd );
		awd->highlighted = sd->curMon;
	}
}


#ifdef __linux
void HideAppWnd( ServData *sd, Wnd *appWnd )
{
	int i;
	if( appWnd->visible == TRUE )
	{
		XUnmapWindow( sd->disp, appWnd->wndHandle );
		XUnmapSubwindows( sd->disp, appWnd->wndHandle );
	}
	appWnd->visible = FALSE;

	for( i = 0; i < appWnd->numSubWnds; i++ )
	{
		appWnd->subWnds[i].visible = FALSE;
	}
}
#endif


void DestroyAppWnd( ServData *sd, Wnd *inWnd )
{
	int i;

	for( i = 0; i < inWnd->numSubWnds; i++ )
	{
		/* Call the destroy function of each child window */
		inWnd->subWnds[i].data.destroywnd( sd->disp, inWnd->subWnds + i );
	}

	DestroyAppWndDat( inWnd->wndData );
	XDestroySubwindows( sd->disp, inWnd->wndHandle );
	free( inWnd->subWnds );

	XFreeGC( sd->disp, inWnd->wndGC );
	XDestroyWindow( sd->disp, inWnd->wndHandle );
}
