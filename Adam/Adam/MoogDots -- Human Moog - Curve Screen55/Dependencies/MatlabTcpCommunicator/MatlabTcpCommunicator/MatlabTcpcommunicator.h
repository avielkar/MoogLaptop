#include "TcpClient.h";
#include "TcpServer.h";
#include "PortsDef.h";

using namespace std;

class MatlabTcpCommunicator
{
private:
	//The TcpClient object to communicate with the Matlab server.
	//
	TcpClient* m_client;
public:
	//Connects all the communication ports between the client and the server.
	//
	void ConnectClientPortsToServer();
};