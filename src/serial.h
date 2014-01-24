#ifndef SERIAL_H
#define SERIAL_H

/* Functions to assist serialising of data structures */

#include <stdio.h>
#include "common.h"

typedef struct
{
	uint8_t *sdata;
	int16_t len;
} SerialData;

/* Macros to get an set bits in an 8 bit datatype. x is the byte to set the bit in, b is the bit number (0 is the 'left-most' bit and 7 the 'right-most' bit), v is the value (1 or 0 ) to set that bit to */
#define getBit8(x,b) ( !!( (x) & ( 1 << (7 - (b) ) ) ) )
#define setBit8(x,b,v) (x = ( (v) << ( 7 - (b) ) ) | ( (x) & ~( 1 << ( 7 - (b) ) ) ) )

/* Debugging function mainly */
void printSerialData( SerialData *inSData, int lower, int upper );

/* Function to test for endian-ness */
boolean isLittleEndian( void );
int bitsRequired( int64_t inNum );

/* Serialising functions. lower is lower bit (0-based), upper is upper bit */
/* Signed functions use one's complement. should never be called with upper - lower < 1 */
void serialiseInt8( SerialData *inSData, int8_t inData, int lower, int upper );
void serialiseUInt8( SerialData *inSData, uint8_t inData, int lower, int upper );
void serialiseInt( SerialData *inSData, int64_t inData, int lower, int upper, int numBytes );
void serialiseUInt( SerialData *inSData, uint64_t inData, int lower, int upper, int numBytes );

/* Deserialising functions. lower is lower bit (0-based index), upper is upper bit */
int8_t deserialiseInt8( const SerialData *inSData, int lower, int upper );
uint8_t deserialiseUInt8( const SerialData *inSData, int lower, int upper );
int64_t deserialiseInt( const SerialData *inSData, int lower, int upper );
uint64_t deserialiseUInt( const SerialData *inSData, int lower, int upper );

#endif
