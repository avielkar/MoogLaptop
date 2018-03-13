#include <WinSock2.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <stdlib.h>

#ifndef TCP_SERVER
#define TCP_SERVER

class TcpServer
{
private:
	SOCKET m_s;
	WSADATA m_w;

public:

	//Listens to a specific port from any client.
	//
	int ListenOnPort(int portNum);

	//Shuts down the socket and closes any connections on it.
	//
	void CloseConnection();
};

#endif