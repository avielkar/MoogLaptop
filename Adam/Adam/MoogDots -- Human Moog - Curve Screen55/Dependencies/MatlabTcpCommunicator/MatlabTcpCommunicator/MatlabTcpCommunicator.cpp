#include "MatlabTcpcommunicator.h"


namespace MatlabCommunicator
{

	MatlabTcpCommunicator::MatlabTcpCommunicator()
	{
		m_client = new TcpClient();
	}

	void MatlabTcpCommunicator::ConnectClientPortsToServer()
	{
		m_client->ConnectToHost(FIRSTPORTA, HOST, TIMEOUT_CONNECTION_REQUEST, TIMEOUT_SENT);
		m_client->ConnectToHost(SECONDPORTA, HOST, TIMEOUT_CONNECTION_REQUEST, TIMEOUT_SENT);

		m_client->ConnectToHost(FIRSTPORTB, HOST, TIMEOUT_CONNECTION_REQUEST, TIMEOUT_SENT);
		m_client->ConnectToHost(SECONDORTB, HOST, TIMEOUT_CONNECTION_REQUEST, TIMEOUT_SENT);

		m_client->ConnectToHost(FIRSTPORTCH, HOST, TIMEOUT_CONNECTION_REQUEST, TIMEOUT_SENT);
		m_client->ConnectToHost(FIRSTPORTCL, HOST, TIMEOUT_CONNECTION_REQUEST, TIMEOUT_SENT);

		m_client->ConnectToHost(SECONDPORTCH, HOST, TIMEOUT_CONNECTION_REQUEST, TIMEOUT_SENT);
		m_client->ConnectToHost(SECONDPORTCL, HOST, TIMEOUT_CONNECTION_REQUEST, TIMEOUT_SENT);

		//wait all connections to be made.
		Sleep(TIMEOUT_CONNECTION_REQUEST);
	}

	void MatlabTcpCommunicator::CloseClientPortsToServer()
	{
		m_client->CloseConnection(FIRSTPORTA);
		m_client->CloseConnection(SECONDPORTA);

		m_client->CloseConnection(FIRSTPORTB);
		m_client->CloseConnection(SECONDORTB);

		m_client->CloseConnection(FIRSTPORTCH);
		m_client->CloseConnection(SECONDPORTCH);

		m_client->CloseConnection(FIRSTPORTCL);
		m_client->CloseConnection(SECONDPORTCL);
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
			if (index == MAXLINEBUFFER - 1)
				return -1;
			buffer[index] = currentReadChar;
			index++;
			cout << currentReadChar;
		} while (currentReadChar != '\n');

		//append the data to the output data.
		data.append(buffer, index - 1);

		return index - 1;
	}

	bool MatlabTcpCommunicator::ReadByte(double timeOut, char& data, u_short port)
	{
		{
			if (m_client->ReadByte(port, data) > 0)
			{
				return true;
			}
		}

		return false;
	}

	int MatlabTcpCommunicator::ReadDouble(double timeOut, double& valueOut, u_short port)
	{
		return m_client->ReadDouble(port, valueOut);
	}

	int MatlabTcpCommunicator::WriteDouble(u_short port, double value)
	{
		return m_client->WriteDouble(port, value);
	}

	int MatlabTcpCommunicator::WriteArray(u_short port, char* data, int size)
	{
		return m_client->WriteByteArray(port, data, size);
	}
}