#include "client.h"
#include <X11/Xlib.h>

static boolean InitAppData( AppData *ad );
static void DestroyAppData( AppData *ad );


int main( void )
{
	int sockfd, bcastsock;
	Message inMessage;
	boolean connected;
	MsgSetResolution msr;
	AppData ad;
	int i;

	if( InitAppData( &ad ) == FALSE )
	{
		perror( "Could not initialise data" );
		return 1;
	}

	if( ( bcastsock = createBroadcastSocket() ) == -1 )
	{
		perror( "Could not create broadcast socket" );
		return 1;	
	}

	if( ( sockfd = createListenSocket( AF_UNSPEC, SOCK_DGRAM, PORT_NUM ) ) == -1 )
	{
		perror( "Listening socket could not be created" );
		close( bcastsock );
		return 1;
	}

	if( setSocketBlock( sockfd ) == -1 )
	{
		perror( "Could not set socket to non-block!" );
		close( bcastsock );
		close( sockfd );
		return 1;
	}

	i = 0;
	printf( "Waiting for connection...\n" );
	do
	{
		if( i % 6 == 0 )
		{
			if( sendBroadcast( bcastsock, PORT_NUM + 1 ) == -1 )
			{
				perror( "Broadcast unsuccessful" );
			}
			i = 1;
		}	
		sleep( WAIT_TIME );
		i++;
	} while( acceptConnection( sockfd ) == FALSE );

	msr.width = getScreenWidth( &ad );
	msr.height = getScreenHeight( &ad );
	if( sendMsg( sockfd, SETRESOLUTION, &msr ) == FALSE )
	{
		perror( "Could not send set resolution message" );
		close( sockfd );
		close( bcastsock );
		return 1;
	}
	printf( "Connected!\n" );

	connected = TRUE;


	while( connected )	
	{
		inMessage.msg = NOMSG;
		recvMsg( sockfd, &inMessage );
		switch( inMessage.msg )
		{
			case KEYPRESS:
				{
					buttonPress( ad.disp, inMessage.data.keypress.keysym );
				}
				break;
			case KEYRELEASE:
				{
					buttonRelease( ad.disp, inMessage.data.keyrelease.keysym );
				}
				printf("Key Released! Keysym: %d\n", inMessage.data.keypress.keysym );
				break;
			case POINTERSET:
				{
					setPointerPos( ad.disp, inMessage.data.pointerset.x, inMessage.data.pointerset.y );
				}
				break;
			case POINTERMOVE:
				{
					movePointer( ad.disp, inMessage.data.pointermove.dx, inMessage.data.pointermove.dy );
				}
				break;
			case POINTERPRESS:
				{
					pointerPress( ad.disp, inMessage.data.pointerpress.button );
				}
				break;
			case POINTERRELEASE:
				{
					pointerRelease( ad.disp, inMessage.data.pointerpress.button );
				}
				break;
			case DISPLAYNUM:
				SetNumWndNumber( &ad.popWnd, inMessage.data.displaynum.monNum );
				ShowNumWnd( &ad, &ad.popWnd );
				break;
			case STOPDISPLAYNUM:
				HideNumWnd( &ad, &ad.popWnd );	
				break;
			case CONNECT:
				/* Already connected??? */
				sendMsg( sockfd, DENY, NULL );
				break;
			case DISCONNECT:
				printf("Disconnecting\n");
				connected = FALSE;
				break;
			case CLOSE:
				printf("Application closing!\n");
				connected = FALSE;
				break;
			case NOMSG:	
				#ifdef __linux
				usleep( SLEEP_TIME );
				#endif
				break;
			default:
				fprintf( stderr, "Received bad data: %d\n", inMessage.msg );
				break;
		}	
		UpdateNumWnd( &ad, &ad.popWnd );
	}

	DestroyAppData( &ad );
	close( sockfd );

	return 0;
}


boolean InitAppData( AppData *ad )
{
	ad->disp = XOpenDisplay( NULL );
        if( ad->disp == NULL )
	{
		return FALSE;
	}	

	CreateNumWnd( ad, &ad->popWnd );

	return TRUE;
}


void DestroyAppData( AppData *ad )
{
	DestroyNumWnd( ad, &ad->popWnd );
	XCloseDisplay( ad->disp );
}
