#include "MatlabTcpcommunicator.h"

void MatlabTcpCommunicator::ConnectClientPortsToServer()
{
	m_client = new TcpClient();

	m_client->ConnectToHost(FIRSTPORTA, HOST, TIMEOUT);
	m_client->ConnectToHost(SECONDPORTA, HOST, TIMEOUT);

	m_client->ConnectToHost(FIRSTPORTB, HOST, TIMEOUT);
	m_client->ConnectToHost(SECONDORTB, HOST, TIMEOUT);

	m_client->ConnectToHost(FIRSTPORTCH, HOST, TIMEOUT);
	m_client->ConnectToHost(SECONDPORTCH, HOST, TIMEOUT);

	m_client->ConnectToHost(FIRSTPORTCL, HOST, TIMEOUT);
	m_client->ConnectToHost(SECONDPORTCL, HOST, TIMEOUT);

	//wait all connections to be made.
	Sleep(TIMEOUT);
}

int MatlabTcpCommunicator::ReadString(double timeOut, string& data, u_short port)
{
	char buffer[MAXLINEBUFFER];
	char currentReadChar;
	int index = 0;

	do
	{
		if (!ReadByte(timeOut, currentReadChar, port))
			return -1;
		buffer[index] = currentReadChar;
		index++;
	} while (currentReadChar != '/n');

	//append the data to the output data.
	data.append(buffer , index -1);

	return index - 1;
}

bool MatlabTcpCommunicator::ReadByte(double timeOut, char& data, u_short port)
{
	Timer timer;
	timer.start();

	while (timer.isTimeout(timeOut))
	{
		if (m_client->ReadByte(port, data))
		{
			return true;
		}
	}

	return false;
}
