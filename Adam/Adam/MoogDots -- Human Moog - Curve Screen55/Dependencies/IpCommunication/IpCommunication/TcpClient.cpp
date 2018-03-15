#include "TcpClient.h"

TcpClient::TcpClient()
{

}

bool TcpClient::ConnectToHost(u_short port, const char * ip)
{
	//start the winsock
	WSADATA wsadata;
	int error = WSAStartup(0x202, &wsadata);

	if (error)
		return false;

	//if not the requested version clean up the winsocket.
	if (wsadata.wVersion != 0x202)
	{
		WSACleanup();
		return false;
	}

	//socket adress information
	sockaddr_in target;


	target.sin_family = AF_INET;
	target.sin_port = htons(port);
	target.sin_addr.S_un.S_addr = inet_addr(ip);


	//create socket
	m_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//could not create a socket
	if (m_s == INVALID_SOCKET)
	{
		return false;
	}


	//try connecting......
	if (connect(m_s, (SOCKADDR*)(&target), sizeof(target)) == SOCKET_ERROR)
	{

		return false;
	}

	return true;
}

void TcpClient::CloseConnection()
{
	//close the socket if exists.
	if (m_s)
		closesocket(m_s);

	//clean up winsock.
	WSACleanup();
}

int TcpClient::Write(const char* data)
{
	return send(m_s, data, sizeof(data), 0);
}