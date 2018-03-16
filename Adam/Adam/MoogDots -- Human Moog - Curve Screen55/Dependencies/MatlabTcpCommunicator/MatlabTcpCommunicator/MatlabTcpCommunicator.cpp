#include "TcpClient.h";
#include "TcpServer.h";
#include "PortsDef.h"


int main()
{
	TcpServer* tcpServer = new TcpServer();
	tcpServer->ListenOnPort(8888);

	tcpServer->ListenOnPort(8889);
	char buffer[1024];

	while (true)
	{
		Sleep(100);
		cout << tcpServer->Read(buffer, 8888);
		cout << tcpServer->Read(buffer, 8889);
	}

	tcpServer->CloseConnection(8888);
}