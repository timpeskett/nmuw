#ifndef MSGS_H
#define MSGS_H
/* Structures for message data and respective widths of those structures. singletons are created of the widths. functions are declared for serialising and deserialising each message */

#include "serial.h"
#include "common.h"
#include <stdlib.h>
#include "widdata.h"

/* Used to set a new value for the width of a field. example SETMSGFIELDWID( MsgKeyPress, keynum, 7 ) sets keynum to 7 bits */

typedef enum { KEYPRESS=0, KEYRELEASE, POINTERSET, POINTERMOVE, POINTERPRESS, POINTERRELEASE, SETRESOLUTION, DISPLAYNUM, STOPDISPLAYNUM, CONNECT, DISCONNECT, ALLOW, DENY, CLOSE, NOMSG } MsgType;


IMPORT_WIDTHS;

typedef struct
{
	unsigned int keysym;
} MsgKey;
typedef MsgKey MsgKeyPress;
typedef MsgKey MsgKeyRelease;

typedef struct
{
	int x, y;
} MsgPointerSet;

typedef struct
{
	int dx, dy;
} MsgPointerMove;


typedef struct
{
	unsigned int button;
} MsgPointer, MsgPointerPress, MsgPointerRelease;


typedef struct 
{
	unsigned int width;
	unsigned int height;
} MsgSetResolution;

typedef struct
{
	int monNum;
} MsgDisplayNum;

/* Union to hold message data of all messages */
typedef union
{
	MsgKeyPress keypress;
	MsgKeyRelease keyrelease;
	MsgPointerSet pointerset;
	MsgPointerMove pointermove;
	MsgPointerPress pointerpress;
	MsgPointerRelease pointerrelease;
	MsgSetResolution setresolution;
	MsgDisplayNum displaynum;
	uint8_t *noformat;
} MsgData;

/* Struct to hold a single message of any type */
typedef struct
{
	MsgType msg;
	MsgData data;
} Message;

/* Typedef for function pointers to serialising functions. The functions take a pointer to the struct to serialise and a pointer to the struct which says how wide each field is */
typedef SerialData *(*SerFunc)( const void * );

typedef void (*DeserFunc)( const SerialData *, MsgData * );


/* Used for both MSGKEYPRESS and MSGKEYRELEASE */
SerialData *serialiseMsgKey( const MsgKeyPress *inKey );
void deserialiseMsgKey( const SerialData *inData, MsgData *outMsg );

SerialData *serialiseMsgPointerSet( const MsgPointerSet *inPointerSet );
void deserialiseMsgPointerSet( const SerialData *inData, MsgData *outMsg );

SerialData *serialiseMsgPointerMove( const MsgPointerMove *inPointerMove );
void deserialiseMsgPointerMove( const SerialData *inData, MsgData *outMsg );

/*Used for both MSGPOINTERPRESS and MSGPOINTERRELEASE */
SerialData *serialiseMsgPointer( const MsgPointer *inPointer );
void deserialiseMsgPointer( const SerialData *inData, MsgData *outMsg );

SerialData *serialiseMsgSetResolution( const MsgSetResolution *inSetResolution );
void deserialiseMsgSetResolution( const SerialData *inData, MsgData *outMsg );

SerialData *serialiseMsgDisplayNum( const MsgDisplayNum *inDisplayNum );
void deserialiseMsgDisplayNum( const SerialData *inData, MsgData *outMsg );
#endif
