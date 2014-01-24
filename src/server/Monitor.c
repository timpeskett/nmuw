#include "Monitor.h"


static boolean doesOverlap( Monitor *mon1, Monitor *mon2 );


Monitor *createMonitor( struct sockaddr *addr, int addr_size )
{
	Monitor *newMon;

	newMon = malloc( sizeof( *newMon ) );

	if( addr != NULL )
	{
		newMon->addr = malloc( addr_size );
		memcpy( newMon->addr, addr, addr_size );
	}
	else
	{
		newMon->addr = NULL;
	}

	newMon->addr_size = addr_size;

	newMon->x = newMon->y = newMon->width = newMon->height = 0;

	newMon->sockfd = -1;

	newMon->procs = NULL;
	newMon->numProcs = 0;

	return newMon;
}


int getMonitorFromPoint( Monitor *mon[], int monLen, int x, int y )
{
	int i;
	boolean found = FALSE;

	i = 0;
	while( i < monLen && found == FALSE )
	{
		if( mon[i] != NULL )
		{
			found = isPointInMonitor( mon[i], x, y );
		}
		i++;
	}
	i--;

	if( found == FALSE )
	{
		i = -1;
	}

	return i;
}

void setMonitorDimensions( Monitor *mon[], int monLen, int numMon, int width, int height, double threshold )
{
	mon[numMon]->width = width;
	mon[numMon]->height = height;
	
	setMonitorPosition( mon, monLen, numMon, mon[numMon]->x, mon[numMon]->y, threshold );
}

/* monLen -- length of array. numMon -- index into array to set */
void setMonitorPosition( Monitor *mon[], int monLen, int numMon, int x, int y, double threshold )
{
	int i; 
	int left, top, right, bottom;
	Monitor *monTemp;
	boolean overlapping;

	mon[numMon]->x = x;
	mon[numMon]->y = y;

	i = 0;
	overlapping = FALSE;
	while( i < monLen && overlapping == FALSE )
	{
		if( mon[i] != NULL && mon[i] != mon[numMon] )
		{
			overlapping = doesOverlap( mon[numMon], mon[i] );
		}
		i++;
	}

	/* Remove overlaps */
	if( overlapping == TRUE )
	{
		int dleft, dtop, dright, dbot;

		/* Remove current monitor so it doesn't contribute to bounding box */
		monTemp = mon[numMon];
		mon[numMon] = NULL;
		getBoundingRectangle( mon, monLen, &left, &top, &right, &bottom );
		mon[numMon] = monTemp;

		dleft = mon[numMon]->x + mon[numMon]->width - left;
		dtop = mon[numMon]->y + mon[numMon]->height - top;
		dright = right - mon[numMon]->x;
		dbot = bottom - mon[numMon]->y;

		if( MINARG( absint( dleft ), absint( dtop ), absint( dright ), absint( dbot ) )  == absint( dleft ) )
		{
			mon[numMon]->x = left - mon[numMon]->width - 1;
		}
		else if( MINARG( absint( dleft ), absint( dtop ), absint( dbot ), absint( dright ) ) == absint( dtop ) )
		{
			mon[numMon]->y = top - mon[numMon]->height - 1;
		}
		else if( MINARG( absint( dleft ), absint( dtop ), absint( dbot ), absint( dright ) ) == absint( dbot ) )
		{
			mon[numMon]->y = bottom + 1;
		}
		else
		{
			mon[numMon]->x = right + 1;
		}
	}

	/* Now that overlaps are gone, snap the rectangles together to make sure that at least one of their sides are touching */
	for( i = 0; i < monLen; i++ )
	{
		int dleft, dtop, dright, dbot, j;
		dleft = dtop = dright = dbot = 0;
		if( mon[i] != NULL )
		{
			for( j = 0; j < monLen; j++ )
			{
				if(  mon[j] != NULL && mon[j] != mon[i] )
				{
					if( dleft == 0 || absint(  mon[j]->x + mon[j]->width - mon[i]->x ) < absint( dleft ) )
					{
						dleft = mon[i]->x - ( mon[j]->x + mon[j]->width );
					}
					if( dright == 0 || absint( mon[j]->x - ( mon[i]->x + mon[i]->width ) ) < absint( dright ) )
					{ 
						dright = mon[j]->x - ( mon[i]->x + mon[i]->width );
					}
					if( dtop == 0 || absint( mon[j]->y + mon[j]->height - mon[i]->y ) < absint( dtop ) )
					{
						dtop =  mon[i]->y - ( mon[j]->y + mon[j]->height );
					}
					if( dbot == 0 || absint( mon[j]->y - (mon[i]->y + mon[i]->height ) ) < absint( dbot ) )
					{
						dbot = mon[j]->y - ( mon[i]->y + mon[i]->height );
					}
				}	
			}
			if( MINARG( absint( dleft ), absint( dtop ), absint( dbot ), absint( dright ) ) == absint( dleft ) || absint( dleft ) < mon[i]->width * threshold )
			{
				if( dleft != 0 ) /* 0 means no other monitors present */
				{
					mon[i]->x = mon[i]->x - dleft + 1;
				}	
			}

			if( MINARG( absint( dleft ), absint( dtop ), absint( dbot ), absint( dright ) ) == absint( dtop ) || absint( dtop ) < mon[i]->height * threshold )
			{
				if( dtop != 0 )
				{
					mon[i]->y = mon[i]->y - dtop + 1;
				}
			}

			if( MINARG( absint( dleft ), absint( dtop ), absint( dbot ), absint( dright ) ) == absint( dbot ) || absint( dbot ) < mon[i]->height * threshold )
			{
				if( dbot != 0 )
				{
					mon[i]->y = mon[i]->y + dbot - 1;
				}
			}

			if( MINARG( absint( dleft ), absint( dtop ), absint( dbot ), absint( dright ) ) == absint( dright ) || absint( dright ) < mon[i]->width * threshold  )
			{
				if( dright != 0 )
				{
					mon[i]->x = mon[i]->x + dright - 1;
				}
			}
			printf(" Monitor %d final position: x = %d y = %d\n", i, mon[i]->x, mon[i]->y );
		}
	}

	monTemp = mon[numMon];
	mon[numMon] = NULL;
	getBoundingRectangle( mon, monLen, &left, &top, &right, &bottom );
	mon[numMon] = monTemp;
	/* At least one side is in line, now ensure that they are actually touching */
	/*for( i = 0; i < monLen; i++ )
	{
		if( mon[i] != NULL && mon[i] != mon[numMon] )
		{
			Check to see which edge of bounding box it is against and then which edge of rectangle it is against. Once found, align this monitor with the found monitor 
			if( ( left == mon[numMon]->x + mon[numMon]->width + 1 && mon[numMon]->x + mon[numMon]->width - mon[i]->x == -1 ) || ( right == mon[numMon]->x - 1 && mon[numMon]->x - ( mon[i]->x + mon[i]->width ) == 1 ) )
			{
				printf("Snapping 1\n");
				if( ( mon[numMon]->y + mon[numMon]->width < mon[i]->y ) || ( mon[numMon]->y > mon[i]->y + mon[i]->height ) )
				{
					mon[numMon]->y = mon[i]->y;
				}
			}
			else if( ( top == mon[numMon]->y + mon[numMon]->width + 1 && mon[numMon]->y + mon[numMon]->height - mon[i]->y == -1 ) || ( bottom == mon[numMon]->y - 1 && mon[numMon]->y - ( mon[i]->y + mon[i]->height ) == 1 ))
			{
				printf("Snapping 2\n");
				if( ( mon[numMon]->x + mon[numMon]->width < mon[i]->x ) || ( mon[numMon]->x > mon[i]->x + mon[i]->width ) )
				{
					mon[numMon]->x = mon[i]->x;
				}
			}
		}
	}*/
}
		

void getMonitorPosition( Monitor *mon[], int monLen, int numMon, int *left, int *top, int *right, int *bot )
{
	if( mon[numMon] != NULL )
	{
		*left = mon[numMon]->x;
		*top = mon[numMon]->y;
		*right = mon[numMon]->x + mon[numMon]->width;
		*bot = mon[numMon]->y + mon[numMon]->height;
	}
}

void getBoundingRectangle( Monitor *mon[], int monLen, int *left, int *top, int *right, int *bottom )
{
	int i;
	boolean set = FALSE;

	*left = *top = *right = *bottom = 0;

	for( i = 0; i < monLen; i++ )
	{
		if( mon[i] != NULL )
		{
			if( set == FALSE )
			{
				set = TRUE;
				*left = mon[i]->x;
				*top = mon[i]->y;
				*right = mon[i]->x + mon[i]->width;
				*bottom = mon[i]->y + mon[i]->height;
			}
			else
			{
				if( mon[i]->x < *left )
				{
					*left = mon[i]->x;
				}
				if( mon[i]->x + mon[i]->width > *right )
				{
					*right = mon[i]->x + mon[i]->width;
				}
				if( mon[i]->y < *top )
				{
					*top = mon[i]->y;
				}
				if( mon[i]->y + mon[i]->height > *bottom )
				{
					*bottom = mon[i]->y + mon[i]->height;
				}
			}
		}
	}
}

void getMonitorDimensions( Monitor *mon[], int monLen, int numMon, int *width, int *height )
{
	*width = *height = 0;
	if( mon[numMon] != NULL )
	{
		*width = mon[numMon]->width;
		*height = mon[numMon]->height;
	}
}



boolean doesOverlap( Monitor *mon1, Monitor *mon2 )
{
	boolean overlap = FALSE;
	int l1, r1, t1, b1;
	int l2, r2, t2, b2;

	l1 = mon1->x; r1 = mon1->x + mon1->width;
	t1 = mon1->y; b1 = mon1->y + mon1->height;
	l2 = mon2->x; r2 = mon2->x + mon2->width;
	t2 = mon2->y; b2 = mon2->y + mon2->height;
	
	if( isPointInMonitor( mon1, l2, t2 ) || isPointInMonitor( mon1, r2, b2 ) || isPointInMonitor( mon1, r2, t2 ) || isPointInMonitor( mon1, l2, b2 ) )
	{
		overlap = TRUE;
	}
	/* Need to check both cases in case one is wholly encased inside other */
	else if( isPointInMonitor( mon2, l1, t1 ) || isPointInMonitor( mon2, r1, b1 ) || isPointInMonitor( mon2, r1, t1) || isPointInMonitor( mon2, r1, b1 ) )
	{
		overlap = TRUE;
	}
	/* cover cases where monitors form a 'cross' */
	else if( t1 < t2 && b1 > b2 && l1 > l2 && r1 < r2 )
	{
		overlap = TRUE;
	}
	else if( t2 < t1 && b2 > b1 && l2 > l1 && r2 < r1 )
	{
		overlap = TRUE;
	}

	return overlap;
}


boolean isPointInMonitor( Monitor *mon1, int x, int y )
{
	boolean inside = FALSE;

	if( ( mon1->x <= x && x <= mon1->x + mon1->width ) && ( mon1->y <= y && y <= mon1->y + mon1->height ) )
	{
		inside = TRUE;
	}

	return inside;
}


void pointerPosToMonitorPos( Monitor *mon[], int monLen, int numMon, int *x, int *y )
{
	if( mon[numMon] != NULL )
	{
		*x += mon[numMon]->x;
		*y += mon[numMon]->y;
	}
}

void monitorPosToPointerPos( Monitor *mon[], int monLen, int *x, int *y )
{
	int monNum;

	monNum = getMonitorFromPoint( mon, monLen, *x, *y );
	if( monNum != -1 )
	{
		int left, top, right, bot;

		getMonitorPosition( mon, monLen, monNum, &left, &top, &right, &bot );

		*x -= left;
		*y -= top;
	}
	else
	{
		*x = *y = -1;
	}
}


boolean connectMonitor( Monitor *inMon )
{
	boolean success = FALSE;

	if( ( inMon->sockfd = createSocket( AF_INET, SOCK_DGRAM ) ) != -1 )
	{
		if( connectSocket( inMon->sockfd, inMon->addr, inMon->addr_size, PORT_NUM ) == TRUE  )
		{
			sendMsg( inMon->sockfd, CONNECT, NULL );
			success = TRUE;
		}
	}

	if( success == FALSE )
	{
		close( inMon->sockfd );
		inMon->sockfd = -1;
	}

	return success;
}




void destroyMonitor( Monitor *inMon )
{
	if( inMon != NULL )
	{
		if( inMon->sockfd != -1 )
		{
			sendMsg( inMon->sockfd, DISCONNECT, NULL );
			close( inMon->sockfd );
			inMon->sockfd = -1;
		}

		free( inMon->addr );
		free( inMon->procs );
	}

	free( inMon );
}


void displayMonitorNumber( Monitor *inMon, int num, boolean display )
{
	MsgDisplayNum mdn;
	assert( inMon != NULL );

	if( display == TRUE )
	{
		mdn.monNum = num;
		sendMsg( inMon->sockfd, DISPLAYNUM, &mdn );
	}
	else
	{
		sendMsg( inMon->sockfd, STOPDISPLAYNUM, NULL );
	}
}

int countActiveMonitors( Monitor *mon[], int monLen )
{
	int i;
	int monCount = 0;

	for( i = 0; i < monLen; i++ )
	{
		if( mon[i] != NULL )
		{
			monCount++;
		}
	}

	return monCount;
}
