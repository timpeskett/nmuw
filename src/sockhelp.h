#ifndef SOCKHELP_H
#define SOCKHELP_H

#ifdef __linux
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <sys/poll.h>
#endif
#include <string.h>
#include <stdio.h>
#include "common.h"

/* -1 on error, valid socket number on success */
int createSocket( int fam, int type );

int createListenSocket( int fam, int type, unsigned int port );

boolean connectSocket( int sockfd, struct sockaddr *inAddr, socklen_t addrLen, int port );

boolean acceptConnection( int sockfd );

boolean waitForData( int sockfd, int timeout );

int createBroadcastSocket( void  );

int sendBroadcast( int sockfd, unsigned int port );

int recvBroadcast( int sockfd, struct sockaddr_in *addr );

int setSocketBlock( int sockfd );

#endif
