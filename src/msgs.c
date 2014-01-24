#include "msgs.h"


const SerFunc serArray[] =
{
	(SerFunc)serialiseMsgKey,
	(SerFunc)serialiseMsgKey,
	(SerFunc)serialiseMsgPointerSet,
	(SerFunc)serialiseMsgPointerMove,
	(SerFunc)serialiseMsgPointer,
	(SerFunc)serialiseMsgPointer,
	(SerFunc)serialiseMsgSetResolution,
	(SerFunc)serialiseMsgDisplayNum
}; const unsigned int serNumEnt = sizeof( serArray ) / sizeof( *serArray );


const DeserFunc deserArray[] =
{
	deserialiseMsgKey,
	deserialiseMsgKey,
	deserialiseMsgPointerSet,
	deserialiseMsgPointerMove,
	deserialiseMsgPointer,
	deserialiseMsgPointer,
	deserialiseMsgSetResolution,
	deserialiseMsgDisplayNum
}; const unsigned int deserNumEnt = sizeof( deserArray ) / sizeof( *deserArray );


SerialData *serialiseMsgKey( const MsgKey *inKey )
{
	SerialData *outData;	

	outData = malloc( sizeof( *outData ) );
	outData->len = ( MSGFIELD( MsgKey, keysym ) + MSGFIELD( MsgKey, state ) ) / 8 + 1;
	outData->sdata = malloc( outData->len  * sizeof( *outData->sdata ) );
	serialiseUInt( outData, inKey->keysym, 0, MSGFIELD( MsgKey, keysym ) - 1, sizeof( inKey->keysym ) );
        return outData;
}


void deserialiseMsgKey( const SerialData *inData, MsgData *outMsg )
{
	/* keypress and keyrelease are in a union so it doesn't matter which one we modify */
	outMsg->keypress.keysym = deserialiseUInt( inData, 0, MSGFIELD( MsgKey, keysym ) - 1 );
}


SerialData *serialiseMsgPointerSet( const MsgPointerSet *inPointerSet )
{
	SerialData *outData;

	outData = malloc( sizeof( *outData ) );
	outData->len = ( MSGFIELD( MsgPointerSet, x ) + MSGFIELD( MsgPointerSet, y ) ) / 8 + 1;
	outData->sdata = malloc( outData->len * sizeof( *outData->sdata ) );
	serialiseInt( outData, inPointerSet->x, 0, MSGFIELD( MsgPointerSet, x ) - 1, sizeof( inPointerSet->x ) );
	serialiseInt( outData, inPointerSet->y, MSGFIELD( MsgPointerSet, x ), MSGFIELD( MsgPointerSet, x ) + MSGFIELD( MsgPointerSet, y ) - 1, sizeof( inPointerSet->y ) );

	return outData;
}


void deserialiseMsgPointerSet( const SerialData *inData, MsgData *outMsg )
{
	outMsg->pointerset.x = deserialiseInt( inData, 0, MSGFIELD( MsgPointerSet, x ) - 1 );
	outMsg->pointerset.y = deserialiseInt( inData, MSGFIELD( MsgPointerSet, x ), MSGFIELD( MsgPointerSet, x ) + MSGFIELD( MsgPointerSet, y ) - 1 );
}

SerialData *serialiseMsgPointerMove( const MsgPointerMove *inPointerMove )
{
	SerialData *outData;

	outData = malloc( sizeof( *outData ) );
	outData->len = ( MSGFIELD( MsgPointerMove, dx ) + MSGFIELD( MsgPointerMove, dy ) ) / 8 + 1;
	outData->sdata = malloc( outData->len * sizeof( *outData->sdata ) );

	serialiseInt( outData, inPointerMove->dx, 0, MSGFIELD( MsgPointerMove, dx ) - 1, sizeof( inPointerMove->dx ) );
	serialiseInt( outData, inPointerMove->dy, MSGFIELD( MsgPointerMove, dx ), MSGFIELD( MsgPointerMove, dx ) + MSGFIELD( MsgPointerMove, dy ) - 1, sizeof( inPointerMove->dy ) );
	return outData;
}


void deserialiseMsgPointerMove( const SerialData *inData, MsgData *outMsg )
{
	outMsg->pointermove.dx = deserialiseInt( inData, 0, MSGFIELD( MsgPointerMove, dx ) - 1 );
	outMsg->pointermove.dy = deserialiseInt( inData, MSGFIELD( MsgPointerMove, dx ), MSGFIELD( MsgPointerMove, dx ) + MSGFIELD( MsgPointerMove, dy ) - 1 );
}


SerialData *serialiseMsgPointer( const MsgPointer *inPointer )
{
	SerialData *outData;

	outData = malloc( sizeof( *outData ) );
	outData->len = ( MSGFIELD( MsgPointer, button ) + MSGFIELD( MsgPointer, state ) ) / 8 + 1;
	outData->sdata = malloc( outData->len * sizeof( *outData->sdata ) );

	serialiseUInt( outData, inPointer->button, 0, MSGFIELD( MsgPointer, button ) - 1, sizeof( inPointer->button ) );

	return outData;
}


void deserialiseMsgPointer( const SerialData *inData, MsgData *outMsg )
{
	/* pointerpress and pointerrelease are same so we can write to either */
	outMsg->pointerpress.button = deserialiseUInt( inData, 0, MSGFIELD( MsgPointer, button ) - 1 );
}


SerialData *serialiseMsgSetResolution( const MsgSetResolution *inSetResolution )
{
	SerialData *outData;

	outData = malloc( sizeof( *outData ) );
	outData->len = ( MSGFIELD( MsgSetResolution, width ) + MSGFIELD( MsgSetResolution, height ) ) / 8 + 1;
	outData->sdata = malloc( outData->len * sizeof( *outData->sdata ) );

	serialiseUInt( outData, inSetResolution->width, 0, MSGFIELD( MsgSetResolution, width ) - 1, sizeof( inSetResolution->width ) );
	serialiseUInt( outData, inSetResolution->height, MSGFIELD( MsgSetResolution, width ), MSGFIELD( MsgSetResolution, width ) + MSGFIELD( MsgSetResolution, height ) - 1, sizeof( inSetResolution->height ) );

	return outData;
}


void deserialiseMsgSetResolution( const SerialData *inData, MsgData *outMsg )
{
	outMsg->setresolution.width = deserialiseUInt( inData, 0, MSGFIELD( MsgSetResolution, width ) - 1 );
	outMsg->setresolution.height = deserialiseUInt( inData, MSGFIELD( MsgSetResolution, width ), MSGFIELD( MsgSetResolution, width ) + MSGFIELD( MsgSetResolution, height ) - 1 );
}


SerialData *serialiseMsgDisplayNum( const MsgDisplayNum *inDisplayNum )
{
	SerialData *outData;

	outData = malloc( sizeof( *outData ) );
	outData->len = ( MSGFIELD( MsgDisplayNum, monNum ) ) / 8 + 1;
        outData->sdata = malloc( outData->len * sizeof( *outData->sdata ) );

	serialiseInt( outData, inDisplayNum->monNum, 0, MSGFIELD( MsgDisplayNum, monNum ) - 1, sizeof( inDisplayNum->monNum ) );

	return outData;
}	


void deserialiseMsgDisplayNum( const SerialData *inData, MsgData *outMsg )
{
	outMsg->displaynum.monNum = deserialiseInt( inData, 0, MSGFIELD( MsgDisplayNum, monNum ) - 1 );
}
