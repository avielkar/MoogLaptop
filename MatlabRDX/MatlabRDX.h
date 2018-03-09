#pragma once

#include <cbw.h>
#include <time.h>
#include <iostream>

using namespace std;

class CMatlabRDX
{
private:
	int m_boardNum;

public:
	CMatlabRDX(int boardNum);

	// Initializes the client connection by configuring all needed ports.
	// This must be done prior to communication with the server.
	int InitClient(int inControlPort, int dataPort, int outControlPort);

	// Reads a string from the communication port.
	int ReadString(double timeOut, int bufferSize, string *data, int inControlPort, int dataPort, int outControlPort);
};