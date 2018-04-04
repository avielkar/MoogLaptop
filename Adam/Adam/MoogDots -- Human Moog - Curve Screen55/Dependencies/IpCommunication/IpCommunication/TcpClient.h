// MatlabTcpCommunicator.cpp : Defines a Tcp clent.
//


#include <iostream>

#include <WinSock2.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <stdlib.h>

#include "Timer.h"

#include <thread>

#include <map>


#ifndef TCP_CLIENT
#define TCP_CLIENT

using namespace std;

//Target host details:
#define HOST "127.0.0.1"

namespace IpCommunication
{
	class TcpClient
	{

	private:
		//The ports map to their data socket.
		//
		std::map<int, SOCKET> m_portsDataSocketMap;

		//Connectin to a given host threded function.
		//port - the port num to connect to.
		//ip - the ip string to connect to with the given port.
		//timeOut - timeOut time for retreiving connection in ms.
		//timeOutSocket - timeout time in ms for receving data requests. 
		bool ConnectToHostThread(u_short port, const char * ip, double timeOut, double timeOutSocket);

	public:
		//Default constructor.
		//
		TcpClient();

		//Connectin to a given host.
		//port - the port num to connect to.
		//ip - the ip string to connect to with the given port.
		//timeOutRequest - timeOut time in ms for retreiving connection in ms.
		//timeOutSocket - timeout time in ms for receving data requests. 
		bool ConnectToHost(u_short port, const char * ip, double timeOutRequest, double timeOutSocket);

		//Shuts down the socket and closes any connections on it.
		//
		void CloseConnection(u_short port);

		//Send data to the server.
		//
		int Write(int port, const char* data);

		//Read a single char from the buffer communication in the given port.
		//port - The server port to read the data from.
		//data - The read char in the buffer.
		//Returns the numbers of read chars.
		int ReadByte(int port, char& data);

		//Read a single double from the buffer communication in the given port.
		//port - The server port to read the data from.
		//data - The read double in the buffer.
		//Returns the numbers of read chars.
		int ReadDouble(int port, double& data);
	
	private:
		//Read a specific number of bytes from the buffer communication in the given port (blocking).
		//port - The server port to read the data from.
		//data - The read bytes in the buffer.
		//numberOfBytes - The number of bytes to wait (block) and read from the buffer.
		//Returns the numbers of read chars.
		int ReadBytes(int port, char* data, int numberOfBytes);
	};
}

#endif

