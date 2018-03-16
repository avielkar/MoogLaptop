#include <WinSock2.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <stdlib.h>

#include <thread>

#include <map>

#ifndef TCP_SERVER
#define TCP_SERVER


//A full tcp server for sendinf and receivinf data.
class TcpServer
{
private:
	//The socket fomr listening to connection requests.
	//SOCKET m_s;

	//The socket for listening to a specific user and to read and write data.
	//SOCKET m_dataSocket;

	//The ports map to their listening socket.
	std::map<int, SOCKET> m_portsListeningSocketMap;

	//The ports map to their data socket.
	std::map<int, SOCKET> m_portsDataSocketMap;

protected:
	int ListenOnPortThread(int port);

public:
	//Default constructor.
	//
	TcpServer();

	//Listens to a specific port from any client.
	//
	int ListenOnPort(int portNum);

	//Shuts down the socket and closes any connections on it.
	//
	void CloseConnection(int port);

	//Read bytes from buffer.
	//
	int Read(char* buffer , int port);
};

#endif