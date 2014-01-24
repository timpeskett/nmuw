#include "listWndChild.h"

/* This is required so the parent knows how many subwindows are required and how to create them. After creation, the update and destroy functions can be accessed through the subwindow's data */
const ChildCreate listWndChildren[] = { listWndConnectCreate, listWndCancelCreate };
const int listWndChildNum = sizeof( listWndChildren ) / sizeof( *listWndChildren );

static const windata list_wnd_connect = { 70, 90, 14, 8, 1, ExposureMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask, CWOverrideRedirect | CWBorderPixel | CWBackPixel | CWEventMask, InputOutput, CopyFromParent, False, listWndConnectCreate, (ChildUpdate)listWndConnectUpdate, listWndConnectDestroy };
static const windata list_wnd_cancel = { 85, 90, 14, 8, 1, ExposureMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask, CWOverrideRedirect | CWBorderPixel | CWBackPixel | CWEventMask, InputOutput, CopyFromParent, False, listWndCancelCreate, (ChildUpdate)listWndCancelUpdate, listWndCancelDestroy };



/****************************************************
 * 		CONNECT BUTTON
 */
void listWndConnectCreate( Display *disp, Wnd *parent, Wnd *outWnd )
{
	outWnd->data = list_wnd_connect;

	percentToAbsoluteDim( outWnd, parent->data.width, parent->data.height );
	CreateWindowHelper( disp, parent, &outWnd->data, outWnd );
	outWnd->wndGC = parent->wndGC; 
}

void listWndConnectUpdate( ServData *sd, XEvent *evt, Wnd *subWnd )
{
	const char label[] = "Connect";
	int selected, i;
	boolean freeMon = FALSE;
	ListWndDat *wndData;

	wndData = (ListWndDat*)sd->listWnd.wndData;

	switch( evt->type )
	{
		case Expose:
			drawCenteredText( sd->disp, subWnd, label, sizeof( label ) - 1 );
			break;
		case ButtonPress:
			selected = wndData->selected;
			if( selected != 0 )
			{
				for( i = 0; i < MAX_MON; i++ )
				{
					if( sd->mons[i] == NULL )
					{
						freeMon = TRUE;
					}
				}
				if( freeMon == TRUE )
				{
					Monitor *newMon;
					Message inMessage;

					newMon = createMonitor( (struct sockaddr*)&wndData->addrs[selected - 1], sizeof( wndData->addrs[selected - 1] ) );

					if( connectMonitor( newMon ) == TRUE )
					{
						if( waitForData( newMon->sockfd, 1000 ) == TRUE )
						{
							recvMsg( newMon->sockfd, &inMessage );
							if( inMessage.msg == SETRESOLUTION )
							{
								i = 0;
								while( sd->mons[i] != NULL && i < MAX_MON )
								{
									i++;	
								}
								sd->mons[i] = newMon;
								setMonitorDimensions( sd->mons, MAX_MON, i, inMessage.data.setresolution.width, inMessage.data.setresolution.height, 0.1 );
								/* Clear list so that connected entry no longer displays */
								free( wndData->addrs );
								wndData->addrs = NULL;
								wndData->numEntries = 0;
								XClearArea( sd->disp, sd->listWnd.wndHandle, 0, 0, 0, 0, True );
							}
							else
							{
								perror( "No resolution recieved" );
								destroyMonitor( newMon );
							}
						}
						else
						{
							perror( "Waiting for resolution failed" );
							destroyMonitor( newMon );
						}
					}
					else
					{
						perror( "Could not connect monitor" );
						destroyMonitor( newMon );
					}
				}
				wndData->selected = 0;	
			}
			break;
		default:
			break;
	}
}


void listWndConnectDestroy( Display *disp, Wnd *inWnd )
{
	/* GC belongs to parent, we just borrow it. no need for us to free it */
	XDestroyWindow( disp, inWnd->wndHandle );
}



/*********************************************************
 * 			CANCEL BUTTON
 */
void listWndCancelCreate( Display *disp, Wnd *parent, Wnd *outWnd )
{
	outWnd->data = list_wnd_cancel;

	percentToAbsoluteDim( outWnd, parent->data.width, parent->data.height );
	CreateWindowHelper( disp, parent, &outWnd->data, outWnd );
	outWnd->wndGC = parent->wndGC; 
}


void listWndCancelUpdate( ServData *sd, XEvent *evt, Wnd *subWnd )
{
	const char label[] = "Cancel";

	switch( evt->type )
	{
		case Expose:
			drawCenteredText( sd->disp, subWnd, label, sizeof( label ) - 1 );
			break;
		case ButtonRelease:
			HideListWnd( sd, &sd->listWnd );
			break;
		default:
			break;
	}
}

void listWndCancelDestroy( Display *disp, Wnd *inWnd )
{
	/* GC belongs to parent, we just borrow it. no need for us to free it */
	XDestroyWindow( disp, inWnd->wndHandle );
}


