#include "TcpClient.h";
#include "TcpServer.h";
#include "PortsDef.h"


int main()
{
	TcpClient* client = new TcpClient();

	client->ConnectToHost(FIRSTPORTA, HOST, TIMEOUT);
	client->ConnectToHost(SECONDPORTA, HOST, TIMEOUT);

	client->ConnectToHost(FIRSTPORTB, HOST, TIMEOUT);
	client->ConnectToHost(SECONDORTB, HOST, TIMEOUT);

	client->ConnectToHost(FIRSTPORTCH, HOST, TIMEOUT);
	client->ConnectToHost(SECONDPORTCH, HOST, TIMEOUT);

	client->ConnectToHost(FIRSTPORTCL, HOST, TIMEOUT);
	client->ConnectToHost(SECONDPORTCL, HOST, TIMEOUT);

	//wait all connections to be made.
	Sleep(TIMEOUT);

	client->CloseConnection(FIRSTPORTA);
	client->CloseConnection(SECONDPORTA);

	client->CloseConnection(FIRSTPORTB);
	client->CloseConnection(SECONDORTB);

	client->CloseConnection(FIRSTPORTCH);
	client->CloseConnection(SECONDPORTCH);

	client->CloseConnection(FIRSTPORTCL);
	client->CloseConnection(SECONDPORTCL);
}
