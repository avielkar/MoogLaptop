#include "PortsDef.h"
#include "MatlabTcpcommunicator.h"


int main()
{
	MatlabTcpCommunicator* matlabTcpCommunicator = new MatlabTcpCommunicator();
	matlabTcpCommunicator->ConnectClientPortsToServer();

	string data;

	matlabTcpCommunicator->ReadString(10000, data, FIRSTPORTA);

	matlabTcpCommunicator->CloseClientPortsToServer();
}
