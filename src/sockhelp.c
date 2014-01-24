#include "sockhelp.h"

#ifdef __linux
int createSocket( int fam, int type )
{
	return socket( fam, type, 0  ); 
}
#endif


#ifdef __linux
boolean connectSocket( int sockfd, struct sockaddr *inAddr, socklen_t addrLen, int port )
{
	boolean success = FALSE;
	struct sockaddr_in addr;
	struct sockaddr_in6 addr6;

	if( addrLen == sizeof( addr ) )
	{
		memcpy( &addr, inAddr, addrLen );
		addr.sin_port = htons( port );
		inAddr = (struct sockaddr*)&addr;
	}
	else
	{
		memcpy( &addr6, inAddr, addrLen );
		addr6.sin6_port = htons( port );
		inAddr = (struct sockaddr*)&addr6;
	}

	if( connect( sockfd, inAddr, addrLen ) == 0 )
	{
		success = TRUE;
	}

	return success;
}
#endif


#ifdef __linux
boolean acceptConnection( int sockfd )
{
	char buf[4];
	struct sockaddr_storage inAddr;
	socklen_t inAddrSize;
	boolean success = FALSE;

	inAddrSize = sizeof( inAddr );
	if( recvfrom( sockfd, &buf, sizeof( buf ), 0, (struct sockaddr*)&inAddr, &inAddrSize ) == sizeof( buf ) )
	{
		if( connect( sockfd, (struct sockaddr*)&inAddr, inAddrSize ) == 0 )
		{
			success = TRUE;
		}
	}

	return success;
}
#endif


#ifdef __linux
boolean waitForData( int sockfd, int timeout )
{
	struct pollfd sock;
	boolean success = FALSE; 

	sock.fd = sockfd;
	sock.events = POLLIN;

	if( poll( &sock, 1, timeout ) > 0 )
	{
		success = TRUE;
	}

	return success;
}
#endif


#ifdef __linux
int createListenSocket( int fam, int type, unsigned int port )
{
	struct addrinfo hints, *local, *it;
	char numString[10];
	int sockfd;
	int success;

	assert( port > 0 && port <= 999999 );

	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = fam;
	hints.ai_socktype = type;
	hints.ai_flags = AI_PASSIVE;
	sprintf( numString, "%d", port );
	if( ( success = getaddrinfo( NULL, numString, &hints, &local ) ) == 0 )
	{
		for( it = local; it != NULL; it = it->ai_next )
		{
			if( ( sockfd = socket( it->ai_family, it->ai_socktype, it->ai_protocol ) ) != -1 )
			{
				if( bind( sockfd, it->ai_addr, it->ai_addrlen ) != -1 )
				{
					break;
				}
				else
				{
					close( sockfd );
				}
			}
		}
		freeaddrinfo( local );
	}
	else
	{
		fprintf( stderr, "Error occured in getaddrinfo: %s\n", gai_strerror( success ) );
	}

	return sockfd;
}
#endif


#ifdef __linux
int createBroadcastSocket( void )
{
	int sockfd, bcast;

	if( ( sockfd = socket( AF_INET, SOCK_DGRAM, 0 ) ) == -1 )
	{
		return -1;
	}

	bcast = 1;
	if( setsockopt( sockfd, SOL_SOCKET, SO_BROADCAST, &bcast, sizeof( bcast ) ) == -1 )
	{
		close( sockfd );
		return -1;
	}

	return sockfd;
}
#endif


#ifdef __linux
int sendBroadcast( int sockfd, unsigned int port )
{
	struct sockaddr_in bcastaddr;
	unsigned char data = 1;
	socklen_t len;

	bcastaddr.sin_family = AF_INET;
	bcastaddr.sin_port = htons( port );
	inet_pton( bcastaddr.sin_family, "255.255.255.255", &bcastaddr.sin_addr ); 
	memset( bcastaddr.sin_zero, '\0', sizeof( bcastaddr.sin_zero ) );

	len = sizeof( bcastaddr );
	if( sendto( sockfd, &data, sizeof( data ), 0, (struct sockaddr*)&bcastaddr, len ) != sizeof( data ) )
	{
		return -1;
	}

	return 0;
}
#endif

#ifdef __linux
int recvBroadcast( int sockfd, struct sockaddr_in *addr )
{
	socklen_t addrSize;
	char buf[10];
	int bytesRead;

	addrSize = sizeof( *addr );
	bytesRead = recvfrom( sockfd, &buf, sizeof( buf ), 0, (struct sockaddr*)addr, &addrSize );
	if( bytesRead == 1 && buf[0] == 1 )
	{
		return 0;
	}

	return 1;
}
#endif


#ifdef __linux
int setSocketBlock( int sockfd )
{
	return fcntl( sockfd, F_SETFL, O_NONBLOCK );
}
#endif
