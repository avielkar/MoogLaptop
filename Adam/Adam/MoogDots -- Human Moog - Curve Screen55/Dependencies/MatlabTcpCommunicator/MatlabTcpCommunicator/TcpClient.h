// MatlabTcpCommunicator.cpp : Defines a Tcp clent.
//

#include "stdafx.h"

#include <iostream>

#include <WinSock2.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <stdlib.h>




using namespace std;

//Target host details:
#define PORT 1234
#define HOST "127.0.0.1"


class TcpClient
{

private:

	SOCKET m_s;

public:

	//Default constructor.
	//
	TcpClient();

	//Connectin to a given host.
	//port - the port num to connect to.
	//ip - the ip string to connect to with the given port.
	bool ConnectToHost(u_short port, const char * ip);

	//Shuts down the socket and closes any connections on it.
	//
	void CloseConnection();
};

