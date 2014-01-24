#include "CaptWnd.h"


#ifdef __linux
static const windata cap_window_data = { 0, 0, 100, 100, 0, ButtonPressMask | KeyPressMask | KeyReleaseMask | ButtonReleaseMask | PointerMotionMask, CWOverrideRedirect | CWEventMask, InputOnly, 0, True, NULL, NULL, NULL };
#endif


static CaptWndDat *CreateCaptWndDat( ServData *sd );
static void DestroyCaptWndDat( ServData *sd, CaptWndDat *cwd );


void CreateCaptWnd( ServData *sd, Wnd *outWnd )
{
	int dispWidth, dispHeight;

	getMonitorDimensions( sd->mons, MAX_MON, MASTER, &dispWidth, &dispHeight );	

	/* Heights are stored as percentages rather than pixels. Convert to pixels here */
	outWnd->data = cap_window_data;
	outWnd->data.width = ( outWnd->data.width / 100.0 ) * dispWidth;
	outWnd->data.height = ( outWnd->data.height / 100.0 ) * dispHeight;

	CreateWindowHelper( sd->disp, NULL, &outWnd->data, outWnd );

	/* Initialise window-specific data */
	outWnd->wndData = CreateCaptWndDat( sd );
}


/*Created at window creation */
CaptWndDat *CreateCaptWndDat( ServData *sd )
{
	CaptWndDat *cwd;
	Pixmap mask, curs;
	Drawable cursSurf;
	unsigned int cursWidth, cursHeight;
	XColor fore, back;
	GC myGC;
	XGCValues attrs;

	cwd = malloc( sizeof( *cwd ) );

	cwd->keyboardCapt = cwd->pointerCapt = FALSE;
	cwd->lastx = cwd->lasty = -1;
	cwd->margin = 100;
	cwd->threshold = 5;

	cursSurf = DefaultRootWindow( sd->disp );
	XQueryBestCursor( sd->disp, cursSurf, 32, 32, &cursWidth, &cursHeight );

	mask = XCreatePixmap( sd->disp, cursSurf, cursWidth, cursHeight, 1 );
	curs = XCreatePixmap( sd->disp, cursSurf, cursWidth, cursHeight, 1 );
	
	attrs.foreground = 0;
	attrs.background = 0;
	attrs.fill_style = FillSolid;
	myGC = XCreateGC( sd->disp, mask, GCForeground | GCBackground | GCFillStyle, &attrs ); 

	XFillRectangle( sd->disp, mask, myGC, 0, 0, cursWidth, cursHeight );
	XFillRectangle( sd->disp, curs, myGC, 0, 0, cursWidth, cursHeight );

	memset( &fore, 0, sizeof( fore ) );
	memset( &back, 0, sizeof( back ) );
	fore.flags = back.flags = DoRed | DoGreen | DoBlue;

	/* Only the mask (set to all zeros for max alpha) is important here */
	cwd->transCurs = XCreatePixmapCursor( sd->disp, mask, mask, &fore, &back, 0, 0 );

	XFreeGC( sd->disp, myGC );

	XFreePixmap( sd->disp, mask );
	XFreePixmap( sd->disp, curs );

	return cwd;
}


void CaptKeyboard( ServData *sd, Wnd *captWnd )
{
	if( captWnd->visible == FALSE )
	{
		ShowCaptWnd( sd, captWnd );
	}

	if( ( (CaptWndDat*)captWnd->wndData)->keyboardCapt == FALSE )
	{
		XGrabKeyboard( sd->disp, captWnd->wndHandle, False, GrabModeAsync, GrabModeAsync, CurrentTime );
		( (CaptWndDat*)captWnd->wndData)->keyboardCapt = TRUE;
	}
}

void UncaptKeyboard( ServData *sd, Wnd *captWnd )
{
	if( ( (CaptWndDat*)captWnd->wndData)->keyboardCapt == TRUE )
	{
		XUngrabKeyboard( sd->disp, CurrentTime );
		( (CaptWndDat*)captWnd->wndData)->keyboardCapt = FALSE;
	}
}


void CaptPointer( ServData *sd, Wnd *captWnd )
{
	CaptWndDat * wndDat;

	if( captWnd->visible == FALSE )
	{
		ShowCaptWnd( sd, captWnd );
	}

	wndDat = captWnd->wndData;
	if( wndDat->pointerCapt == FALSE )
	{
		XGrabPointer( sd->disp, captWnd->wndHandle, False, ButtonPressMask | PointerMotionMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, captWnd->wndHandle, wndDat->transCurs, CurrentTime );
		wndDat->pointerCapt = TRUE;
	}
}


void UncaptPointer( ServData *sd, Wnd *captWnd )
{
	if( ( (CaptWndDat*)captWnd->wndData)->pointerCapt == TRUE )
	{
		XUngrabPointer( sd->disp, CurrentTime );
		( (CaptWndDat*)captWnd->wndData)->pointerCapt = FALSE;
	}
}


void ShowCaptWnd( ServData *sd, Wnd *captWnd )
{
	if( captWnd->visible == FALSE )
	{
		XMapWindow( sd->disp, captWnd->wndHandle );
	}
	captWnd->visible = TRUE;
}



void UpdateCaptWnd( ServData *sd, Wnd *captWnd )
{
	XEvent evt;
	CaptWndDat *wndDat;	

	/* Just a variable to access the window data to save typing */
	wndDat = captWnd->wndData;

	if( XEventsQueued( sd->disp, QueuedAlready ) != 0 )
	{
		if( XCheckWindowEvent( sd->disp, captWnd->wndHandle, captWnd->data.event_mask, &evt ) == True )
		{
			switch( evt.type )
			{
				case ButtonPress:
				case ButtonRelease:
					{
						int pointerMon;
						MsgPointer mpp; /* Handle both down and up events */

						pointerMon = getMonitorFromPoint( sd->mons, MAX_MON, sd->pointerx, sd->pointery );

						/* Check to see if we need to capture the keyboard after a focus change and set the newly focussed monitor */
						if( pointerMon != -1 )
						{
							if( sd->focusMon == MASTER && pointerMon != MASTER )
							{
								/* Focus has changed away, grab keyboard */
								CaptKeyboard( sd, captWnd );
							}
							else if( sd->focusMon != MASTER && pointerMon == MASTER )
							{
								/* Focus is back! ungrab keyboard */
								UncaptKeyboard( sd, captWnd );
							}
							sd->lastFocus = sd->focusMon;
							sd->focusMon = sd->curMon = pointerMon;
						}

						/* Send message to appropriate monitor */
						switch( pointerMon )
						{
							case -1:
								break;
							case MASTER:
								HideCaptWnd( sd, captWnd );
								/* Input needs to be repeated because it wasn't caught that time */

								printf("Repeating input\n");
								if( evt.type == ButtonPress )
								{
									pointerPress( sd->disp, evt.xbutton.button );
								}
								else
								{
									pointerRelease( sd->disp, evt.xbutton.button );
								}
								break;
							default:
								mpp.button = evt.xbutton.button;

								if( evt.type == ButtonPress )
								{
									sendMsg( sd->mons[pointerMon]->sockfd, POINTERPRESS, &mpp );
								}
								else
								{
									sendMsg( sd->mons[pointerMon]->sockfd, POINTERRELEASE, &mpp );
								}
								break;
						}	
					}
					break;
				case KeyPress:
					{
						if( evt.xkey.keycode == XKeysymToKeycode( sd->disp, XK_Q ) && evt.xkey.state & Mod1Mask && evt.xkey.state & ControlMask  )
						{
							MsgKeyRelease mkr;

							/* CTRL + ALT have been sent through but we are changing focus so we need to send the release messages. Send both left and right because we don't know which the user hit and X doesn't tell us :( */
							printf("Sending release message to %d\n", sd->focusMon );
							mkr.keysym = XK_Control_L; 
							sendMsg( sd->mons[sd->focusMon]->sockfd, KEYRELEASE, &mkr );
							mkr.keysym = XK_Alt_L;
							sendMsg( sd->mons[sd->focusMon]->sockfd, KEYRELEASE, &mkr );
							mkr.keysym = XK_Control_R;
							sendMsg( sd->mons[sd->focusMon]->sockfd, KEYRELEASE, &mkr );
							mkr.keysym = XK_Alt_R;
							sendMsg( sd->mons[sd->focusMon]->sockfd, KEYRELEASE, &mkr );


							sd->focusMon = MASTER; /* DONT set curMon here, we are only changing focus temporarily */
							/* If the keyboard is captured then we need to uncapture it */
							UncaptKeyboard( sd, captWnd ); 
							ToggleAppWnd( sd, &sd->appWnd );

														break;
						}
					}
					/* Intentional switch-case fall-through */
				case KeyRelease:
					{
						KeySym sym;
						MsgKey mk;

						switch( sd->focusMon )
						{
							case -1:
							case MASTER:
								/* Error has occurred, don't send keypress */
								break;
							default:
								sym = XLookupKeysym( &evt.xkey, 0 );
								mk.keysym = sym;
								
								sendMsg( sd->mons[sd->focusMon]->sockfd, evt.type == KeyPress ? KEYPRESS : KEYRELEASE, &mk );

								break;
						}
					}
					break;
				case MotionNotify:
					{
						int newMon;
						int newx, newy;
						int margin;
						int screenw, screenh;
						int lastMon;
						MsgPointerMove mpm;

						/* Messages still being processed when pointer is not captured, stop this */
						if( wndDat->pointerCapt == FALSE )
						{
							break;
						}

						mpm.dx = evt.xmotion.x_root - wndDat->lastx;
						mpm.dy = evt.xmotion.y_root - wndDat->lasty;

						margin = wndDat->margin;
						getMonitorDimensions( sd->mons, MAX_MON, MASTER, &screenw, &screenh );

						/* If dx/dx are the size of a warp then eat them. This may eat some bigger mouse movements unfortunately but is almost unavoidable */
						if( absint( mpm.dx ) > ( screenw - margin ) - screenw / 2 || absint( mpm.dy ) > ( screenh - margin ) - screenh / 2 )
						{
							break;
						}

						/* If pointer nears the edge of the screen then reset it to the middle */
						if(  evt.xmotion.x_root < margin || evt.xmotion.x_root > screenw - margin || evt.xmotion.y_root < margin || evt.xmotion.y_root > screenh - margin )
						{
							XWarpPointer( sd->disp, None, RootWindow( sd->disp, DefaultScreen( sd->disp ) ), 0, 0, 0, 0, screenw / 2, screenh / 2 );
							wndDat->lastx = screenw / 2;
							wndDat->lasty = screenh / 2;
						}
						else
						{
							wndDat->lastx = evt.xmotion.x_root;
							wndDat->lasty = evt.xmotion.y_root;
						}

						lastMon = getMonitorFromPoint( sd->mons, MAX_MON, sd->pointerx, sd->pointery );

						newx = sd->pointerx + mpm.dx;
						newy = sd->pointery + mpm.dy;

						newMon = getMonitorFromPoint( sd->mons, MAX_MON, newx, newy );

						if( newMon != -1 && newMon != MASTER && newMon != lastMon )
						{
							/* Transition to new monitor, set new pointer position */
							MsgPointerSet mps;

							mps.x = newx;
							mps.y = newy;
							monitorPosToPointerPos( sd->mons, MAX_MON, &mps.x, &mps.y );

							sendMsg( sd->mons[newMon]->sockfd, POINTERSET, &mps );
						}
						/* Hide pointer if mouse moved */
						if( lastMon != MASTER && lastMon != -1 && newMon != -1 && lastMon != newMon )
						{
							int left, top, right, bot;
							MsgPointerSet hideCoords;

							getMonitorPosition( sd->mons, MAX_MON, lastMon, &left, &top, &right, &bot );
							hideCoords.x = right - left;
							hideCoords.y = bot - top;
							sendMsg( sd->mons[lastMon]->sockfd, POINTERSET, &hideCoords );
						}



						switch( newMon )
						{
							case -1: /* Out of bounds */
								break;
							case MASTER:
							{
								int left, top, right, bottom;

								sd->pointerx = newx;
								sd->pointery = newy;

								getMonitorPosition( sd->mons, MAX_MON, MASTER, &left, &top, &right, &bottom );
								/* Ensure that pointer is not outside threshold value to be moved onto the next monitor */
								if( sd->pointerx < left + wndDat->threshold )
									sd->pointerx = left + wndDat->threshold;
								if( sd->pointerx > right - wndDat->threshold )
									sd->pointerx = right - wndDat->threshold;
								if( sd->pointery < top + wndDat->threshold )
									sd->pointery = top + wndDat->threshold;
								if( sd->pointery > bottom - wndDat->threshold )
									sd->pointery = bottom - wndDat->threshold;
								
								XWarpPointer( sd->disp, None, RootWindow( sd->disp, DefaultScreen( sd->disp ) ), 0, 0, 0, 0, sd->pointerx - left, sd->pointery - top );
								UncaptPointer( sd, captWnd );
								break;
							}
							default:
								sd->pointerx = newx;
								sd->pointery = newy;
								sendMsg( sd->mons[newMon]->sockfd, POINTERMOVE, &mpm );
								break;
						}
					}	
					break;
				default:
					break;
			}
		}
	}
	
	if(  wndDat->pointerCapt == FALSE )
	{
		int pointerMon;
		int left, top, right, bot;
		int pointerx, pointery;
		int root_x, root_y;
		unsigned int mask_return;
		Window child_return, root_return;

		XQueryPointer( sd->disp, RootWindow( sd->disp, DefaultScreen( sd->disp ) ), &root_return, &child_return, &root_x, &root_y, &sd->pointerx, &sd->pointery, &mask_return );
		
		pointerPosToMonitorPos( sd->mons, MAX_MON, MASTER, &sd->pointerx, &sd->pointery );
		getMonitorPosition( sd->mons, MAX_MON, MASTER, &left, &top, &right, &bot );
		pointerx = sd->pointerx;
		pointery = sd->pointery;
		/* If pointer is at edge of screen, 'push' it over on to the next monitor */
		if( pointerx < left + wndDat->threshold)
		{
			pointerx -= wndDat->threshold;
		}
		if( pointery < top + wndDat->threshold )
		{
			pointery -= wndDat->threshold;
		}
		if( pointerx > right - wndDat->threshold )
		{
			pointerx += wndDat->threshold;
		}
		if( pointery > bot - wndDat->threshold )
		{
			pointery += wndDat->threshold;
		}
		pointerMon = getMonitorFromPoint( sd->mons, MAX_MON, pointerx, pointery );
		if( pointerMon != MASTER && pointerMon != -1 )
		{
			int screenw, screenh;
			MsgPointerSet mps;

			/* Pointer to middle of the screen so that it can still be moved towards that edfe */
			screenw = DisplayWidth( sd->disp, DefaultScreen( sd->disp ) );
			screenh = DisplayHeight( sd->disp, DefaultScreen( sd->disp ) );
			XWarpPointer( sd->disp, None, RootWindow( sd->disp, DefaultScreen( sd->disp ) ), 0, 0, 0, 0, screenw / 2, screenh / 2 );

			ShowCaptWnd( sd, captWnd );
			CaptPointer( sd, captWnd );

			wndDat->lastx = screenw / 2;
			wndDat->lasty = screenh / 2;

			sd->pointerx = pointerx;
			sd->pointery = pointery;

			monitorPosToPointerPos( sd->mons, MAX_MON, &pointerx, &pointery );
			mps.x = pointerx;
			mps.y = pointery;
			sendMsg( sd->mons[pointerMon]->sockfd, POINTERSET, &mps );
		}
	}
}




void HideCaptWnd( ServData *sd, Wnd *captWnd )
{
	if( captWnd->visible == TRUE )
	{
		XUnmapWindow( sd->disp, captWnd->wndHandle );
	}
	captWnd->visible = FALSE;
}


void DestroyCaptWndDat( ServData *sd, CaptWndDat *cwd )
{
	XFreeCursor( sd->disp, cwd->transCurs );
	free( cwd );
}


void DestroyCaptWnd( ServData *sd, Wnd *inWnd )
{
	DestroyCaptWndDat( sd, inWnd->wndData );
	XDestroyWindow( sd->disp, inWnd->wndHandle );
}
