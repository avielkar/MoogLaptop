#include "TcpClient.h"

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

bool::TcpClient::ConnectToHostThread(u_short port, const char * ip)
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


	//try connecting......
	if (connect(s, (SOCKADDR*)(&target), sizeof(target)) == SOCKET_ERROR)
	{
		return false;
	}

	m_portsDataSocketMap[port] = s;

	return true;
}

bool TcpClient::ConnectToHost(u_short port, const char * ip)
{
	std::thread t = thread(&TcpClient::ConnectToHostThread, this, port, ip);
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

int TcpClient::Write(int port , const char* data)
{
	cout << "writing - " << data << " - " << sizeof(data);

	return send(m_portsDataSocketMap[port], data, strlen(data), 0);
}