// MatlabCommunicatorTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "MatlabTcpCommunicator.h"

using namespace MatlabCommunicator;
using namespace IpCommunication;


int _tmain(int argc, _TCHAR* argv[])
{
	MatlabTcpCommunicator* matlabTcpcommunicator = new MatlabTcpCommunicator();

	matlabTcpcommunicator->ConnectClientPortsToServer();

	matlabTcpcommunicator->CloseClientPortsToServer();

	return 0;
}

