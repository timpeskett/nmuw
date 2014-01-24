#include "netmsg.h"


boolean sendAll( int sockfd, const char *data, int datSize, int flags )
{
	boolean success = TRUE;
	int bytesSent;

	while( datSize > 0 && success == TRUE )
	{
		bytesSent = send( sockfd, data, datSize, flags );
		if( bytesSent == -1 )
		{
			success = FALSE;
		}
		else
		{
			datSize -= bytesSent;
			data += bytesSent;
		}
	}

	return success;
}
	

boolean sendMsg( int sockfd, MsgType type, void *dataStruct )
{
	SerialData outDat, *serStruct;
	boolean success = TRUE;
	int bodyLen = 0;

	if( type >= serNumEnt || serArray[type] == NULL )
	{
		/* send data as is, assume that dataStruct first two bytes are length and the rest is the data */
		if( dataStruct != NULL )
		{
			bodyLen = deserialiseInt( dataStruct, 0, LEN_WID * 8 - 1 );
		}
		outDat.len = TYPE_WID + LEN_WID + bodyLen;

		outDat.sdata = malloc( outDat.len * sizeof( *outDat.sdata ) );
		serialiseUInt( &outDat, type, 0, TYPE_WID * 8 - 1, sizeof( type ) );
		serialiseInt( &outDat, bodyLen, TYPE_WID * 8, TYPE_WID * 8 + LEN_WID * 8 - 1, sizeof( bodyLen ) );
		if( dataStruct != NULL )
		{
			memcpy( outDat.sdata + TYPE_WID + LEN_WID, (char*)dataStruct + LEN_WID, outDat.len - TYPE_WID - LEN_WID );
		}
		success = sendAll( sockfd, (char*)outDat.sdata, outDat.len,  0);
	}
	else
	{
		assert( dataStruct != NULL );

		serStruct = serArray[type]( dataStruct );
		outDat.len = serStruct->len + TYPE_WID + LEN_WID;
		outDat.sdata = malloc( ( outDat.len ) * sizeof( *serStruct->sdata ) );
	        serialiseUInt( &outDat, type, 0, TYPE_WID * 8 - 1, sizeof( type ) );
		serialiseInt( &outDat, serStruct->len, TYPE_WID * 8, TYPE_WID * 8 + LEN_WID * 8 - 1, sizeof( serStruct->len ) );
		memcpy( outDat.sdata + TYPE_WID + LEN_WID, serStruct->sdata, serStruct->len );
		success = sendAll( sockfd, (char*)outDat.sdata, outDat.len * sizeof( *outDat.sdata ), 0 );

		free( serStruct->sdata );
		free( serStruct );
		free( outDat.sdata );
	}

	return success;
}	


boolean recvExact( int sockfd, char *buf, int len, int flags )
{
	boolean success = TRUE;
	int bytesRead;

	while( success && len > 0 )
	{
		if( (bytesRead = recv( sockfd, buf, len, flags  ) ) > 0 )
		{
			len -= bytesRead;
			buf += bytesRead;
		}
		else
		{
			success = FALSE;
		}
	}

	return success;
}

boolean recvMsg( int sockfd, Message *outMessage )
{
	int datalen, hdrSize;
	MsgType type;
	char *buf;
	SerialData serDat;
	boolean success = TRUE;

	assert( outMessage != NULL );

	hdrSize = ( TYPE_WID + LEN_WID ) * sizeof( *buf );
	
	buf = malloc( hdrSize  );
	success = recvExact( sockfd, buf, hdrSize, MSG_PEEK );
	if( success == TRUE )
	{
		serDat.sdata = (uint8_t*)buf;
		serDat.len = hdrSize;
		type = deserialiseUInt( &serDat, 0, TYPE_WID * 8 - 1 );
		datalen = deserialiseInt( &serDat, TYPE_WID * 8, TYPE_WID * 8 + LEN_WID * 8 - 1 ) * sizeof( *buf );	
		free( buf );

		datalen += hdrSize;/*Header still in buffer*/
		buf = malloc( datalen );
		success = recvExact( sockfd, buf, datalen, 0 );
		if( success == TRUE )
		{
			if( type >= deserNumEnt || deserArray[type] == NULL )
			{
				/* Messages without deserialisation routines are assumed to have no data */
			}
			else
			{
				serDat.sdata = (uint8_t*)buf + hdrSize;
				serDat.len = datalen - hdrSize;
				deserArray[type]( &serDat, &outMessage->data );
			}
			outMessage->msg = type;
		}
	}
	
	free( buf );

	return success;
}	
