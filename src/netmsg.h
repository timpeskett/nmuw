#ifndef NETMSG_H
#define NETMSG_H

#include "common.h"
#include "serial.h"
#include "msgs.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>

#define TYPE_WID 2
#define LEN_WID 2

/* Arrays that contains the functions to serialise/deserialise all message types. contained in msgs.c for now */
extern const SerFunc serArray[]; extern const unsigned int serNumEnt;
extern const DeserFunc deserArray[]; extern const unsigned int deserNumEnt;

boolean sendAll( int sockfd, const char *data, int datSize, int flags );
/* Pretty simple. The dataStruct containing the message data is passed in along with a function to serialise said data. type is then added to said serialised data (always as a 16-bit integer field) and then data is sent */
boolean sendMsg( int sockfd, MsgType type, void *dataStruct );

boolean recvExact( int sockfd, char *buf, int len, int flags );
boolean recvMsg( int sockfd, Message *outMessage ); 

#endif
