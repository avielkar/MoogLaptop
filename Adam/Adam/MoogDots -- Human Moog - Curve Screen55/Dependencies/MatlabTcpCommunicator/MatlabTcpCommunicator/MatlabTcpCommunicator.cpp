#include "TcpClient.h";
#include "TcpServer.h";


int main()
{
	TcpServer* tcpServer = new TcpServer();
	tcpServer->ListenOnPort(6659);
	
	TcpClient* tcpClient = new TcpClient();
	tcpClient->ConnectToHost(6659, "127.0.0.1");
}