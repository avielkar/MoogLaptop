#include "TcpClient.h";
#include "TcpServer.h";
#include "PortsDef.h";

#ifndef MATLABTCPCOMMUNICATOR
#define MATLABTCPCOMMUNICATOR

using namespace std;
using namespace IpCommunication;

namespace MatlabCommunicator
{
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
		//Default constructor.
		//
		MatlabTcpCommunicator();

		//Connects all the communication ports between the client and the server.
		//
		void ConnectClientPortsToServer();

		//Closes the clients port that communcate with the Matlab server.
		//
		void CloseClientPortsToServer();

		//This function read a string command from the given port (string command is data untill first '/n' in the buffer).
		//timeOut - The max time to try reading the line wothout connection.
		//data - the first command in the buffer.
		//port - the port number to read the command from.
		//Returns the number of read bytes int the line (without the '\n' byte).
		int ReadString(double timeOut, string & data, u_short port);

		//This function read a double value from the buffer communication.
		//timeOut - The max time to try reading the line wothout connection.
		//valueOut - The double value data read from the buffer.
		//port - the port number to read the command from.
		//Returns the number of read bytes.
		int ReadDouble(double timeOut, double& valueOut, u_short port);

		int WriteDouble(u_short port, double value);
	};
}
#endif