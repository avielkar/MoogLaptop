#include <iostream>

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

//http://www.bogotobogo.com/cplusplus/sockets_server_client.php

#include "TcpServer.h"


int TcpServer::ListenOnPort(int portNum)
{
	//Fill in WSA info.
	WSADATA m_w;
	int error = WSAStartup(0x202, &m_w);

	//If winsock could not be started.
	if (error)
		return -1;

	//If the version of the socket is not what we asked for.
	if (m_w.wVersion != 0x202)
	{
		WSACleanup();

		return -2;
	}

	//The adress structure for a tco socket.
	sockaddr_in serverAdress;

	//The adress family and the port for this socket.
	serverAdress.sin_family = AF_INET;
	serverAdress.sin_port = htons(portNum);

	//Accepts a connection from any IP using IPADDR_ANY.
	serverAdress.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	//Create the socket.
	m_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//if can't create a a socket.
	if (m_s == INVALID_SOCKET)
		return -3;

	//if trying to bind the same socket more than once.
	if (bind(m_s, (LPSOCKADDR)&serverAdress, sizeof(serverAdress)) == SOCKET_ERROR)
		return -4;

	listen(m_s, SOMAXCONN);

	sockaddr_in client;
	int clientSize = sizeof(client);
	m_dataSocket =  accept(m_s, (sockaddr*)(&client), &clientSize);
	if (m_dataSocket < 0)
		return -5;

	cout << "accept " << client.sin_port;

	return true;
}

void  TcpServer::CloseConnection()
{
	//close the socket if exists.
	if (m_dataSocket)
		closesocket(m_dataSocket);

	if (m_s)
		closesocket(m_s);
		
	//clean up winsock.
	WSACleanup();
}

int TcpServer::Read(char* buffer)
{
	return recv(m_dataSocket, buffer, 1024, 0);
}

