#include "TcpClient.h"

namespace IpCommunication
{
	TcpClient::TcpClient()
	{
		//start the winsock
		WSADATA wsadata;
		int error = WSAStartup(0x202, &wsadata);

		if (error)
			return;

		//if not the requested version clean up the winsocket.
		if (wsadata.wVersion != 0x202)
		{
			WSACleanup();
		}
	}

	bool TcpClient::ConnectToHostThread(u_short port, const char * ip, double timeOut)
	{
		//socket adress information
		sockaddr_in target;
		target.sin_family = AF_INET;
		target.sin_port = htons(port);
		target.sin_addr.S_un.S_addr = inet_addr(ip);


		//create socket
		SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		//could not create a socket
		if (s == INVALID_SOCKET)
		{
			return false;
		}

		Timer timer;
		timer.start();
		while (timer.isTimeout(timeOut))
		{
			//try connecting......
			if (connect(s, (SOCKADDR*)(&target), sizeof(target)) != SOCKET_ERROR)
			{
				cout << "connected to " << port << "\n";
				m_portsDataSocketMap[port] = s;

				//set a timeout for reading from the buffer with the recv function.
				DWORD timeVal = timeOut;
				setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeVal, sizeof(timeVal));

				return true;
			}

			Sleep(200);
		}

		return false;
	}

	bool TcpClient::ConnectToHost(u_short port, const char * ip, double timeOut)
	{
		std::thread t = thread(&TcpClient::ConnectToHostThread, this, port, ip, timeOut);
		t.detach();

		return true;
	}

	void TcpClient::CloseConnection(u_short port)
	{
		//close the socket if exists.
		if (m_portsDataSocketMap[port])
			closesocket(m_portsDataSocketMap[port]);

		//clean up winsock.
		WSACleanup();
	}

	int TcpClient::Write(int port, const char* data)
	{
		cout << "writing - " << data << " - " << sizeof(data);

		return send(m_portsDataSocketMap[port], data, strlen(data), 0);
	}

	int TcpClient::ReadByte(int port, char& data)
	{
		return recv(m_portsDataSocketMap[port], &data, 1, 0);
	}

	int TcpClient::ReadDouble(int port, double& data)
	{
		
	}

	int TcpClient::ReadBytes(int port, char* data, int numberOfBytes)
	{
		int numOfReadBytes = 0;

		while (numberOfBytes < numberOfBytes)
		{
			int m = recv(m_portsDataSocketMap[port], data + numOfReadBytes, numberOfBytes - numOfReadBytes , NULL);

			numOfReadBytes += m;
		}

		return numOfReadBytes;
	}
}