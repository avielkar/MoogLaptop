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