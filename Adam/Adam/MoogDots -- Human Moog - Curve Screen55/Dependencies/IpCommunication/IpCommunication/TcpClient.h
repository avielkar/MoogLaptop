// MatlabTcpCommunicator.cpp : Defines a Tcp clent.
//


#include <iostream>

#include <WinSock2.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <stdlib.h>

#include <thread>

#include <map>


#ifndef TCP_CLIENT
#define TCP_CLIENT

using namespace std;

//Target host details:
#define HOST "127.0.0.1"


class TcpClient
{

private:
	//The ports map to their data socket.
	std::map<int, SOCKET> m_portsDataSocketMap;

	bool ConnectToHostThread(u_short port, const char * ip);

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
	void CloseConnection(u_short port);

	//Send data to the server.
	//
	int Write(int port, const char* data);
};

#endif

