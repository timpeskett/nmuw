#ifndef MONITOR_H
#define MONITOR_H

#include "../common.h"
#include "../msgs.h"
#include "../netmsg.h"
#include "../sockhelp.h"
#include <stdlib.h>
#ifdef __linux
#include <netinet/in.h>
#include <X11/Xlib.h>
#endif

#define MINARG(w,x,y,z) (MIN(w,x)<MIN(y,z)?MIN(w,x):MIN(y,z))
#define MIN(x,y) ((x)<(y)?(x):(y))

typedef struct
{
	int sockfd;
	struct sockaddr* addr;
	int addr_size;
	int x, y; /* 'Position' of monitor in virtual workspace */
	int width, height; /*Dimensions of actual monitor */
        struct
	{
		#ifdef __linux
		Window wnd;
		#endif
		char *wndName;
	} *procs; /* Used to hold data for the processes on a computer */	
	int numProcs;
} Monitor;

Monitor *createMonitor( struct sockaddr *addr, int addr_size );
void setMonitorPosition( Monitor *mon[], int monLen, int numMon, int x, int y, double threshold );
void getMonitorPosition( Monitor *mon[], int monLen, int numMon, int *left, int *top, int *right, int *bot );
void pointerPosToMonitorPos( Monitor *mon[], int monLen, int numMon, int *x, int *y );
void monitorPosToPointerPos( Monitor *mon[], int monLen, int *x, int *y );
int getMonitorFromPoint( Monitor *mon[], int monLen, int x, int y );
void getMonitorDimensions( Monitor *mon[], int monLen, int numMon, int *width, int *height );
void setMonitorDimensions( Monitor *mon[], int monLen, int numMon, int width, int height, double threshold );
void getBoundingRectangle( Monitor *mon[], int monLen, int *left, int *top, int *right, int *bottom );
boolean isPointInMonitor( Monitor *mon1, int x, int y );
boolean connectMonitor( Monitor *inMon );
void destroyMonitor( Monitor *inMon );

/* If display is true, number will be displayed, otherwise it will stop being
 * displayed */
void displayMonitorNumber( Monitor *inMon, int num, boolean display );
int countActiveMonitors( Monitor *mon[], int monLen );

#endif

