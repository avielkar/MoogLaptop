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

	//Read a byte in the timeOut time.
	//Return false if no byte in the given timeOut time , otherwise returns true.
	bool ReadByte(double timeOut, char& data, u_short port);

public:
	//Connects all the communication ports between the client and the server.
	//
	void ConnectClientPortsToServer();

	//This function read a string command from the given port (string command is data untill first '/n' in the buffer).
	//timeOut - The max time to try reading the line wothout connection.
	//data - the fisrst command in the buffer.
	//port - the port number to read the command from.
	//Returns the number of read bytes int he line (without the '\n' byte).
	int ReadString(double timeOut, string & data , u_short port);
};