#include <iostream>

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

//http://www.bogotobogo.com/cplusplus/sockets_server_client.php

#include "TcpServer.h"

TcpServer::TcpServer()
{
	//Fill in WSA info.
	WSADATA w;
	int error = WSAStartup(0x202, &w);

	//If the version of the socket is not what we asked for.
	if (w.wVersion != 0x202)
	{
		WSACleanup();
	}
}

int TcpServer::ListenOnPortThread(int port)
{
	//The adress structure for a tco socket.
	sockaddr_in serverAdress;

	//The adress family and the port for this socket.
	serverAdress.sin_family = AF_INET;
	serverAdress.sin_port = htons(port);

	//Accepts a connection from any IP using IPADDR_ANY.
	serverAdress.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	//Create the socket.
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//if can't create a a socket.
	if (s == INVALID_SOCKET)
		return -3;

	//if trying to bind the same socket more than once.
	if (bind(s, (LPSOCKADDR)&serverAdress, sizeof(serverAdress)) == SOCKET_ERROR)
		return -4;

	listen(s, SOMAXCONN);

	sockaddr_in client;
	int clientSize = sizeof(client);
	SOCKET dataSocket = accept(s, (sockaddr*)(&client), &clientSize);
	if (dataSocket < 0)
		return -5;

	std::cout << "accept " << client.sin_port;

	m_portsListeningSocketMap[port] = s;
	m_portsDataSocketMap[port] = dataSocket;

	return 0;
}

int TcpServer::ListenOnPort(int portNum)
{
	std::thread t(&TcpServer::ListenOnPortThread , this, portNum);
	t.detach();
	return true;
}

void  TcpServer::CloseConnection(int port)
{
	//close the socket if exists.
	if (m_portsDataSocketMap[port])
		closesocket(m_portsDataSocketMap[port]);

	if (m_portsListeningSocketMap[port])
		closesocket(m_portsListeningSocketMap[port]);
		
	//clean up winsock.
	WSACleanup();
}

int TcpServer::Read(char* buffer , int port)
{
	return recv(m_portsDataSocketMap[port], buffer, 1024, 0);
}

