#ifndef LISTWND_H
#define LISTWND_H



#include "../common.h"
#include "../Wnd.h"
#include "../sockhelp.h"
#include <stdio.h>
#include "ServData.h"
#include "Monitor.h"
#include "../wndutil.h"
#ifdef __linux
#include <arpa/inet.h>
#include <sys/types.h>
#include <X11/Xlib.h>
#endif


typedef struct
{
	int selected; /* The selected entry. 0 for none */
	struct sockaddr_in *addrs;
	int numEntries;
	int recvSock;
	int seperator, xmarg, ymarg; /* Used for text formatting */
} ListWndDat;


void CreateListWnd( ServData *sd, Wnd *outWnd );
void ShowListWnd( ServData *sd, Wnd *listWnd );
void UpdateListWnd( ServData *sd, Wnd *listWnd );
void HideListWnd( ServData *sd, Wnd *listWnd );
void DestroyListWnd( ServData *sd, Wnd *inWnd );


#endif
