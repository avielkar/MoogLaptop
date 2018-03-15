#include <WinSock2.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <stdlib.h>

#ifndef TCP_SERVER
#define TCP_SERVER

using namespace std;

//A full tcp server for sendinf and receivinf data.
class TcpServer
{
private:
	//The socket fomr listening to connection requests.
	SOCKET m_s;

	//The socket for listening to a specific user and to read and write data.
	SOCKET m_dataSocket;

public:
	//Default constructor.
	//
	TcpServer();

	//Listens to a specific port from any client.
	//
	int ListenOnPort(int portNum);

	//Shuts down the socket and closes any connections on it.
	//
	void CloseConnection();

	//Read bytes from buffer.
	//
	int Read(char* buffer);
};

#endif