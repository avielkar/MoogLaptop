#include "stdafx.h"

#include <iostream>

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#include "TcpServer.h"

int TcpServer::ListenOnPort(int portNum)
	{
		//Fill in WSA info.
		int error = WSAStartup(0x202, &m_w);

		//If winsock could not be started.
		if (error)
			return false;

		//If the version of the socket is not what we asked for.
		if (m_w.wVersion != 0x202)
		{
			WSACleanup();

			return false;
		}

		//The adress structure for a tco socket.
		sockaddr_in addr;

		//The adress family and the port for this socket.
		addr.sin_family = AF_INET;
		addr.sin_port = htons(portNum);

		//Accepts a connection from any IP using IPADDR_ANY.
		addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

		//Create the socket.
		m_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		//if can't create a a socket.
		if (m_s == INVALID_SOCKET)
			return false;

		//if trying to bind the same socket more than once.
		if (bind(m_s, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
			return false;

		listen(m_s, SOMAXCONN);
	}

void  TcpServer::CloseConnection()
	{
		//close the socket if exists.
		if (m_s)
			closesocket(m_s);

		//clean up winsock.
		WSACleanup();
	}