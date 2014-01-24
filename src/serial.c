#include "serial.h"

void printSerialData( SerialData *inSData, int lower, int upper )
{
	assert( inSData != NULL );

	printf( "Data = " );

	for(; lower <= upper; lower++ )
	{
		printf( "%d, ", getBit8( inSData->sdata[lower/8], lower % 8 ) );
	}
	
	printf("\n" );
}


boolean isLittleEndian( void )
{
	boolean littleEnd;
	uint16_t tester = 1;

	if( *(char*)&tester == 1 )
	{
		littleEnd = TRUE;
	}
	else
	{
		littleEnd = FALSE;
	}

	return littleEnd;
}

/* Number of bits required to store absolute value of inNum */
int bitsRequired( int64_t inNum )
{
	int outBits = 0;
	uint64_t absNum;

	if( inNum == 0 )
	{
		outBits++;/* one bit required if inNum is zero */
	}

	absNum = absint( inNum );

	for(; absNum > 0; absNum = absNum >> 1 )
	{
		outBits++;
	}

	return outBits;
}

/* Serialised into one's complement format. sign bit of 1 means negative */
void serialiseInt8( SerialData *inSData, int8_t inData, int lower, int upper )
{
	assert( inSData != NULL );
	assert( upper - lower + 1 >= bitsRequired( inData ) + 1 );

	if( inData < 0 )
	{
		setBit8( inSData->sdata[lower/8], lower % 8, 1 );
	}
	else
	{
		setBit8( inSData->sdata[lower/8], lower % 8, 0 );
	}

	serialiseUInt8( inSData, absint( inData ), lower + 1, upper );
}

void serialiseUInt8( SerialData *inSData, uint8_t inData, int lower, int upper )
{
	int width;
	width = bitsRequired( inData );
	assert( inSData != NULL );
	assert( upper - lower + 1 >= width );

	for(; upper - lower + 1 > width; lower++ )
	{
		setBit8( inSData->sdata[lower / 8], lower % 8, 0 ); /*pad with zeroes*/
	}
	for(; lower <= upper; lower++ )
	{
		int curBit;
		curBit = getBit8( inData, 7 - ( upper - lower ) );
		setBit8( inSData->sdata[lower / 8], lower % 8, curBit );
	}
}



void serialiseInt( SerialData *inSData, int64_t inData, int lower, int upper, int numBytes )
{
	assert( inSData != NULL );
	assert( upper - lower + 1 >= bitsRequired( inData ) + 1 );
	assert( sizeof( inData ) >= numBytes );

	if( inData < 0 )
	{
		setBit8( inSData->sdata[ lower / 8 ], lower % 8, 1 );
	}
	else
	{
		setBit8( inSData->sdata[ lower / 8 ], lower % 8, 0 );
	}
	
	serialiseUInt( inSData, absint( inData ), lower + 1, upper, numBytes );
}

void serialiseUInt( SerialData *inSData, uint64_t inData, int lower, int upper, int numBytes )
{
	int startByte, step;

	assert( inSData != NULL );
	assert( upper - lower + 1 >= bitsRequired( inData ) );
	assert( sizeof( inData ) >= numBytes );

	while( ( upper - lower + 1 ) / 8 > numBytes ) /* pad extra space with zeroes */
	{
		setBit8( inSData->sdata[ lower / 8 ], lower % 8, 0 );
		lower++;
	}

	if( isLittleEndian() )
	{
		startByte = ( upper - lower ) / 8;
		step = -1;
	}
	else
	{
		startByte = sizeof( inSData ) - ( upper - lower ) / 8;
		step = 1;
	}

	while( lower <= upper )
	{
		if( upper % 8 != 0 )
		{
			serialiseUInt8( inSData, *((uint8_t*)&inData + startByte), lower, lower + ( upper - lower ) % 8 );
			lower += ( upper - lower ) % 8 + 1;
		}
		else
		{
			if( upper - lower == 0 ) /* Handle writing 1 bit */
			{
				serialiseUInt8( inSData, *((uint8_t*)&inData + startByte ), lower, lower );
			}
			else
			{
				serialiseUInt8( inSData, *((uint8_t*)&inData + startByte ), lower, lower + 7 );
			}
			lower += 8;
		}
		startByte += step;
	}
}


uint8_t deserialiseUInt8( const SerialData *inSData, int lower, int upper )
{
	uint8_t outInt = 0;

	assert( inSData != NULL );
	assert( upper - lower >= 0 );

	while( upper - lower > 7 )
	{
		lower++;
	}

	for(; lower <= upper; lower++ )
	{
		int curBit;
		curBit = getBit8( inSData->sdata[lower / 8], lower % 8 );
		setBit8( outInt, 7 - ( upper - lower ), curBit );
	}

	return outInt;
}


int64_t deserialiseInt( const SerialData *inSData, int lower, int upper )
{
	int sign;

	assert( inSData != NULL );
	assert( upper - lower >= 1 );

	if( getBit8( inSData->sdata[lower / 8], lower % 8 ) == 1 )
	{
		sign = -1;
	}
	else
	{
		sign = 1;
	}

	return sign * deserialiseUInt( inSData, lower + 1, upper );
}


uint64_t deserialiseUInt( const SerialData *inSData, int lower, int upper )
{
	uint64_t outInt = 0;
	int startByte, step;

	assert( inSData != NULL );
	assert( upper - lower >= 0 );

	while( ( upper - lower ) / 8 > sizeof( outInt ) - 1 )
	{
		lower++;
	}

	if( isLittleEndian() )
	{
		startByte = ( upper - lower ) / 8;
		step = -1;
	}
	else
	{
		startByte = sizeof( outInt  ) - 1 - ( upper - lower ) / 8;
		step = 1;
	}

	while( lower <= upper )
	{
		if( ( upper - lower ) % 8 == 0 )
		{
			if( lower == upper )
			{
				*((uint8_t*)&outInt + startByte) = deserialiseUInt8( inSData, lower, lower );
			}
			else
			{
				*((uint8_t*)&outInt + startByte) = deserialiseUInt8( inSData, lower, lower + 8 );
			}
			lower += 8;
		}
		else
		{
			*((uint8_t*)&outInt + startByte ) = deserialiseUInt8( inSData, lower, lower + ( upper - lower ) % 8 );
			lower += ( upper - lower ) % 8 + 1;
		}
		startByte += step;
	}
	return outInt;
}
			
