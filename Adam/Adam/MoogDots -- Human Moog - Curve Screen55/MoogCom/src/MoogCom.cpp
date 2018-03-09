#include "stdafx.h"
#include "MoogCom.h"


MoogCom::MoogCom(string mbcIP, int mbcPort, string localIP, int localPort, bool useCustomTimer) :
// Initialization list.
m_continueSending(false), m_packetRate(16.6), m_computeCode(0x00),
m_isEngaged(false), m_doCustomTiming(useCustomTimer), m_doCompute(false),
m_doReceiveCompute(false), m_talker(NULL), m_moogCtrlTiming(false)
{
	// Set the MBC IP address and port.
	m_mbcIP = mbcIP;
	m_mbcPort = mbcPort;

	// Set the local IP address and port.
	m_localIP = localIP;
	m_localPort = localPort;

	// Determine the high performance counter frequency.
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	m_clockFrequency = (double)freq.QuadPart;

	// Initialize the Critical Sections.
	InitializeCriticalSection(&m_comCS);
	InitializeCriticalSection(&m_receiveCS);

	m_syncFrame = false;

	// Initialize the command buffer.
	for (int i = 0; i < PACKET_SIZE; i++) {
		m_commandBuffer[i] = 0x00;
	}

	m_myfile.open("LogFile.txt");

	//The config object expects a full path name and file name eg. "C:/Moog/Motionbase.ini"
	char cFilePath[MAX_PATH + 1];
	strcpy_s(cFilePath, MAX_PATH, "C:\\Moog\\MotionBaseHost.ini");
	CConfigFile* config = new CConfigFile(cFilePath , &m_myfile);
	if (NULL == config)
	{
		ExitProcess(-1);
	}
	else
	{
		m_pScriptFile = new CScriptFile(&m_MBCIF , m_myfile);
	}
	m_MBCIF.Open(config);
#if DOF_MODE
	m_commandBuffer[3] = 0x82;
#else
	m_commandBuffer[3] = 0x80;
#endif

}


void MoogCom::ThreadInit()
{
}


void MoogCom::Control()
{
}


void MoogCom::talker(LPVOID lpParam)
{
	LARGE_INTEGER start, finish;			// Used to clock the thread.
	MoogCom *mcom = (MoogCom*)lpParam;		// Pointer to the parent class.
	SOCKET comSock;							// UDP socket
	WSADATA wsaData;
	SOCKADDR_IN destAddr,
		localAddr;
	ReverseTransform rt;
	int i;
	bool gotData;

	// for debug only
	// FILE *fpt=fopen("c:\\temp\\error.txt","wt");

	// Custom thread initialization.
	mcom->ThreadInit();

	// Initialize the command copy.
	for (i = 0; i < PACKET_SIZE; i++) {
		mcom->m_com[i] = 0x00;
	}

	// Socket initialization.
	// Use the current Winsock DLL, Ws2_32.dll 
	// The Winsock2.h header file for Winsock 2 support
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { // Johnny - 12/5/07
		//if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
		char s[256];
		wsprintf(s, "WSA Startup failed!");
		MessageBox(NULL, s, "Socket Error", MB_OK);
		mcom->m_continueSending = false;
		return;
	}

	// Create the socket.
	if ((comSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) {
		char s[256];
		wsprintf(s, "Socket creation failed with error: %d.\nKilling com thread.", WSAGetLastError());
		MessageBox(NULL, s, "Socket Error", MB_OK);
		mcom->m_continueSending = false;
		return;
	}

	// Set the socket to be nonblocking.
	u_long arg = 1;
	if (ioctlsocket(comSock, FIONBIO, &arg) == SOCKET_ERROR) {
		char s[256];
		wsprintf(s, "Setting socket to nonblocking failed with error: %d.", WSAGetLastError());
		MessageBox(NULL, s, "Socket Error", MB_OK);
		mcom->m_continueSending = false;
		return;
	}

	// Setup the local address and bind to it.
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.S_un.S_addr = inet_addr(mcom->m_localIP.c_str());
	localAddr.sin_port = htons((u_short)mcom->m_localPort);
	if (bind(comSock, (const sockaddr*)&localAddr, sizeof(SOCKADDR_IN)) != 0) {
		char s[256];
		wsprintf(s, "Socket bind failed with error: %d", WSAGetLastError());
		MessageBox(NULL, s, "Socket Error", MB_OK);
		mcom->m_continueSending = false;
		return;
	}

	// Setup the MBC address.
	destAddr.sin_family = AF_INET;
	destAddr.sin_addr.S_un.S_addr = inet_addr(mcom->m_mbcIP.c_str());
	destAddr.sin_port = htons((u_short)mcom->m_mbcPort);

	QueryPerformanceCounter(&finish);
	start = finish;

	bool goNextCommand = true;
	bool commandSent = false;
	while (mcom->m_continueSending) {

		//int xz =  mcom->KeepAlive();

		// This paces the communications.
		if (mcom->m_doCustomTiming) {
			gotData = false;
			goNextCommand = true;

			// Make sure we grab the last packet available.
			while (recvfrom(comSock, (char*)mcom->m_receiveBuffer, RETURNPACKET_SIZE, 0, NULL, NULL) != SOCKET_ERROR) {
				gotData = true;
			} // End while

			if (gotData) {
				// Time stamp the receive time.
				mcom->m_receiveTime = (double)finish.QuadPart;

				// Turn the actuator lengths into useful data.
				for (i = 3; i <= 8; i++) {
					// Extract the return data from the return buffer.
					mcom->m_actuatorData[i - 3] = mcom->ExtractReturnData(i) + ACTUATOR_OFFSET;
				}

				// Convert the actuator lengths into DOF values.
				rt.ReverseTransformMetric(mcom->m_actuatorData, mcom->m_dofValues);

				// Call ReceiveCompute() if needed.
				EnterCriticalSection(&mcom->m_receiveCS);
				if (mcom->m_doReceiveCompute) {
					mcom->ReceiveCompute();
				}
				LeaveCriticalSection(&mcom->m_receiveCS);
			}

			// This function allows derived classes to use a custom timer for control of the
			// communication speed.
			mcom->CustomTimer();

			// Time stamp the send time.
			QueryPerformanceCounter(&finish);
			mcom->m_sendTime = (double)finish.QuadPart;

			// Send the command data.
			/*if (sendto(comSock, (const char*)mcom->m_com, PACKET_SIZE, 0, (const sockaddr*)&destAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
				char s[256];
				wsprintf(s, "Socket send failed with error: %d.", WSAGetLastError());
				MessageBox(NULL, s, "Send Error", MB_OK);
				mcom->m_continueSending = false;
			}*/
		}
		else if (mcom->m_moogCtrlTiming) { // Johnny - 12/5/07
			// Sync the built-in timer to an outside source.
			if (mcom->m_syncFrame == true) {
				mcom->Sync();
				mcom->m_syncFrame = false;
				QueryPerformanceCounter(&start);
			}

			double status = 0; // status is a signal from Moog. If Moog is ready to get command, then we send it immediately

			while (true)
				//while (((double)(finish.QuadPart - start.QuadPart) / mcom->m_clockFrequency * 1000.0) < mcom->m_packetRate) 
			{
				QueryPerformanceCounter(&finish);

				// Grab the return packet.
				if (recvfrom(comSock, (char*)mcom->m_receiveBuffer, RETURNPACKET_SIZE, 0, NULL, NULL) != SOCKET_ERROR) {
					// Time stamp the receive time.
					mcom->m_receiveTime = (double)finish.QuadPart;

					// Try to get ride of bumps.
					if (commandSent){// Command have already sent when we didn't receive feedback after mcom->m_packetRate = 16.66ms.
						goNextCommand = false; // don't update the next command
						commandSent = false;
					}
					// Send the command data immediately.
					/*else if (sendto(comSock, (const char*)mcom->m_com, PACKET_SIZE, 0, (const sockaddr*)&destAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
						char s[256];
						wsprintf(s, "Socket send failed with error: %d.", WSAGetLastError());
						MessageBox(NULL, s, "Send Error", MB_OK);
						mcom->m_continueSending = false;
					}*/

					//goNextCommand = true;

					// Time stamp the send time.
					QueryPerformanceCounter(&finish);
					mcom->m_sendTime = (double)finish.QuadPart;

					// Turn the actuator lengths into useful data.
					for (i = 3; i <= 8; i++) {
						// Extract the return data from the return buffer.
						mcom->m_actuatorData[i - 3] = mcom->ExtractReturnData(i) + ACTUATOR_OFFSET;
					}

					// Convert the actuator lengths into DOF values.
					rt.ReverseTransformMetric(mcom->m_actuatorData, mcom->m_dofValues);

					// Call ReceiveCompute() if needed.
					EnterCriticalSection(&mcom->m_receiveCS);
					if (mcom->m_doReceiveCompute) {
						mcom->ReceiveCompute();
					}
					LeaveCriticalSection(&mcom->m_receiveCS);

					//status = mcom->ExtractReturnData(2);
					//if(status==?) break;

					break;
				}
				// if we don't receive any data, keep communication with moog
				else if (((double)(finish.QuadPart - start.QuadPart) / mcom->m_clockFrequency * 1000.0) > mcom->m_packetRate){
					// record error - for debug only
					// fprintf(fpt,"Socket receive failed with error: %d./n", WSAGetLastError());

					// Send the command data immediately.
					//avi : clean the garbage sending
					/*if (sendto(comSock, (const char*)mcom->m_com, PACKET_SIZE, 0, (const sockaddr*)&destAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
						char s[256];
						wsprintf(s, "Socket send failed with error: %d.", WSAGetLastError());
						MessageBox(NULL, s, "Send Error", MB_OK);
						mcom->m_continueSending = false;
					}*/
					commandSent = true;
					//goNextCommand = false;
					//MessageBox(NULL, "Send Data", "Send first data!", MB_OK);

					break;
				}

				if (!mcom->m_moogCtrlTiming) break;
			}

			if (goNextCommand) start = finish;
		}
		else {
			goNextCommand = true;

			// Sync the built-in timer to an outside source.
			if (mcom->m_syncFrame == true) {
				mcom->Sync();
				mcom->m_syncFrame = false;
				QueryPerformanceCounter(&start);
			}

			while (((double)(finish.QuadPart - start.QuadPart) / mcom->m_clockFrequency * 1000.0) < mcom->m_packetRate) {
				QueryPerformanceCounter(&finish);

				// Grab the return packet.
				if (recvfrom(comSock, (char*)mcom->m_receiveBuffer, RETURNPACKET_SIZE, 0, NULL, NULL) != SOCKET_ERROR) {
					// Time stamp the receive time.
					mcom->m_receiveTime = (double)finish.QuadPart;

					// Turn the actuator lengths into useful data.
					for (i = 3; i <= 8; i++) {
						// Extract the return data from the return buffer.
						mcom->m_actuatorData[i - 3] = mcom->ExtractReturnData(i) + ACTUATOR_OFFSET;
					}

					// Convert the actuator lengths into DOF values.
					rt.ReverseTransformMetric(mcom->m_actuatorData, mcom->m_dofValues);

					// Call ReceiveCompute() if needed.
					EnterCriticalSection(&mcom->m_receiveCS);
					if (mcom->m_doReceiveCompute) {
						mcom->ReceiveCompute();
					}
					LeaveCriticalSection(&mcom->m_receiveCS);
				}
			}
			start = finish;

			// Time stamp the send time.
			QueryPerformanceCounter(&finish);
			mcom->m_sendTime = (double)finish.QuadPart;

			// Send the command data.
			/*if (sendto(comSock, (const char*)mcom->m_com, PACKET_SIZE, 0, (const sockaddr*)&destAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
				char s[256];
				wsprintf(s, "Socket send failed with error: %d.", WSAGetLastError());
				MessageBox(NULL, s, "Send Error", MB_OK);
				mcom->m_continueSending = false;
			}*/
		}

		/*
		// Time stamp the send time.
		QueryPerformanceCounter(&finish);
		mcom->m_sendTime = (double)finish.QuadPart;

		// Send the command data.
		if (sendto(comSock, (const char*)mcom->m_com, PACKET_SIZE, 0, (const sockaddr*)&destAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
		char s[256];
		wsprintf(s, "Socket send failed with error: %d.", WSAGetLastError());
		MessageBox(NULL, s, "Send Error", MB_OK);
		mcom->m_continueSending = false;
		}
		*/

		// Call the Control() function.
		EnterCriticalSection(&mcom->m_comCS);
		mcom->Control();
		LeaveCriticalSection(&mcom->m_comCS);

		// Set which compute functions are called.
		EnterCriticalSection(&mcom->m_comCS);
		if (mcom->m_computeCode & COMPUTE) {
			mcom->m_doCompute = true;
		}
		else {
			mcom->m_doCompute = false;
		}

		if (mcom->m_computeCode & RECEIVE_COMPUTE) {
			mcom->m_doReceiveCompute = true;
		}
		else {
			mcom->m_doReceiveCompute = false;
		}

		// Johnny - 12/13/07
		// 'goNextCommand' is used for 'mcom->m_moogCtrlTiming' that we wait for feedback and then send command.
		// If we don't recieve any feedback, we only send old command and keep communication with Moog.
		// First we don't update 'mcom->m_com' by 'mcom->m_commandBuffer' and
		// we have to stop call 'mcom->Compute()', because it will update next command by SET_DATA_FRAME (ThreadSetAxesPositions)
		// and 'm_data.index++; and m_grabIndex++;' in MoogDatsCom.cpp

		// Execute the Compute() function if needed.
		if (mcom->m_doCompute && goNextCommand) {
			//m_sheet->writeStr(m_xl_row++, 1, "Entering Compute() function.");
			mcom->Compute();
		}
		//m_sheet->writeStr(m_xl_row++, 1, "Exiting Compute() function.");

		// Copy the externally visible command buffer into the one that we actually send over
		// to the MBC.
		if (goNextCommand)
			memcpy(mcom->m_com, mcom->m_commandBuffer, PACKET_SIZE * sizeof(unsigned char));  // Copy the command buffer to the copy array.
		else goNextCommand = true;
		LeaveCriticalSection(&mcom->m_comCS);

		QueryPerformanceCounter(&finish);


	} // End while (mcom->m_continueSending)

	// Close the socket.
	closesocket(comSock);

	// The application or DLL must call WSACleanup to deregister itself from a Windows Sockets implementation
	WSACleanup(); // Johnny - 12/5/07

	// for debug only
	// fclose(fpt);
}

void MoogCom::Sync()
{
}


void MoogCom::SyncNextFrame()
{
	m_syncFrame = true;
}


void MoogCom::UseCustomTimer(bool useCustomTimer)
{
	m_doCustomTiming = useCustomTimer;
}

void MoogCom::UseMoogCtrlTimer(bool useMoogCtrlTiming)
{
	m_moogCtrlTiming = useMoogCtrlTiming;
}


void MoogCom::ThreadSetAxisPosition(Axis axis, float value)
{
	switch (axis)
	{
	case Axis::Heave:
		// Make sure the Heave value is within acceptable range.
#if DOF_MODE
		if (value >= HEAVE_MAX && value <= 0.0f) {
#else
		if (value >= -HEAVE_MAX && value <= HEAVE_MAX) {
#endif
			// Convert the float to network byte order.
			unsigned char *convertedValue = f_nbo_uc(value);

			// Copy the converted value into the command buffer.
			memcpy(m_commandBuffer + HEAVE_INDEX, convertedValue, 4);
			delete[] convertedValue;
		}
		break;
	case Axis::Lateral:
		// Make sure the Lateral value is within acceptable range.
		if (value >= -LATERAL_MAX && value <= LATERAL_MAX) {
			// Convert the float to network byte order.
			unsigned char *convertedValue = f_nbo_uc(value);

			// Copy the converted value into the command buffer.
			memcpy(m_commandBuffer + LATERAL_INDEX, convertedValue, 4);
			delete[] convertedValue;
		}
		break;
	case Axis::Surge:
		// Make sure the Surge value is within acceptable range.
		if (value >= -SURGE_MAX && value <= SURGE_MAX) {
			// Convert the float to network byte order.
			unsigned char *convertedValue = f_nbo_uc(value);

			// Copy the converted value into the command buffer.
			memcpy(m_commandBuffer + SURGE_INDEX, convertedValue, 4);
			delete[] convertedValue;
		}
		break;
	case Axis::Yaw:
		// Make sure the Roll value is within acceptable range.
		if (value >= -YAW_MAX && value <= YAW_MAX) {
			// Convert the value from degrees into radians.
			value *= (float)(PI / 180.0);

			// Convert the float to network byte order.
			unsigned char *convertedValue = f_nbo_uc(value);

			// Copy the converted value into the command buffer.
			memcpy(m_commandBuffer + YAW_INDEX, convertedValue, 4);
		}
		break;
	case Axis::Pitch:
		// Make sure the Roll value is within acceptable range.
		if (value >= -PITCH_MAX && value <= PITCH_MAX) {
			// Convert the value from degrees into radians.
			value *= (float)(PI / 180.0);

			// Convert the float to network byte order.
			unsigned char *convertedValue = f_nbo_uc(value);

			// Copy the converted value into the command buffer.
			memcpy(m_commandBuffer + PITCH_INDEX, convertedValue, 4);
			delete[] convertedValue;
		}
		break;
	case Axis::Roll:
		// Make sure the Roll value is within acceptable range.
		if (value >= -ROLL_MAX && value <= ROLL_MAX) {
			// Convert the value from degrees into radians.
			value *= (float)(PI / 180.0);

			// Convert the float to network byte order.
			unsigned char *convertedValue = f_nbo_uc(value);

			// Copy the converted value into the command buffer.
			memcpy(m_commandBuffer + ROLL_INDEX, convertedValue, 4);
			delete[] convertedValue;
		}
		break;
		};
	}

#include <iostream>
void MoogCom::ThreadSetAxesPositions(MoogFrame *moogFrame)
{

	/*
		Step 1 : (old , only for saving the commandBuffer for after that to save to the currentPositions parameter , not sending them).
		This step is here(although it was in there communication) because it sets the values of the commandBuffer for the cuurrent place of the MBC.
		There is a funcction called ThreadGetAxesPositions(...) which after this call takes the values of commandBuffer and put it in the currentPosition parameter.
		So the parameters in this variable must be updated from the commandBuffer values.
	*/

	// Make sure the Lateral value is within acceptable range.
	if (moogFrame->lateral >= -LATERAL_MAX && moogFrame->lateral <= LATERAL_MAX) {
		// Convert the float to network byte order.
		unsigned char *convertedValue = f_nbo_uc(moogFrame->lateral);

		// Copy the converted value into the command buffer.
		memcpy(m_commandBuffer + LATERAL_INDEX, convertedValue, 4);
		delete[] convertedValue;
	}

	// Make sure the Surge value is within acceptable range.
	if (moogFrame->surge >= -SURGE_MAX && moogFrame->surge <= SURGE_MAX) {
		// Convert the float to network byte order.
		unsigned char *convertedValue = f_nbo_uc(moogFrame->surge);

		// Copy the converted value into the command buffer.
		memcpy(m_commandBuffer + SURGE_INDEX, convertedValue, 4);
		delete[] convertedValue;
	}

	// Make sure the Heave value is within acceptable range.
#if DOF_MODE
	if (moogFrame->heave >= HEAVE_MAX && moogFrame->heave <= 0.0f) {
#else
	if (moogFrame->heave >= -HEAVE_MAX && moogFrame->heave <= HEAVE_MAX) {
#endif
		// Convert the float to network byte order.
		unsigned char *convertedValue = f_nbo_uc(moogFrame->heave);

		// Copy the converted value into the command buffer.
		memcpy(m_commandBuffer + HEAVE_INDEX, convertedValue, 4);
		delete[] convertedValue;
	}

	// Make sure the Yaw value is within acceptable range.
	if (moogFrame->yaw >= -YAW_MAX && moogFrame->yaw <= YAW_MAX) {
		// Convert the value from degrees into radians.
		float value = moogFrame->yaw;
		value *= (float)(PI / 180.0);

		// Convert the float to network byte order.
		unsigned char *convertedValue = f_nbo_uc(value);

		// Copy the converted value into the command buffer.
		memcpy(m_commandBuffer + YAW_INDEX, convertedValue, 4);
		delete[] convertedValue;
	}

	// Make sure the Pitch value is within acceptable range.
	if (moogFrame->pitch >= -PITCH_MAX && moogFrame->pitch <= PITCH_MAX) {
		// Convert the value from degrees into radians.
		float value = moogFrame->pitch;
		value *= (float)(PI / 180.0);

		// Convert the float to network byte order.
		unsigned char *convertedValue = f_nbo_uc(value);

		// Copy the converted value into the command buffer.
		memcpy(m_commandBuffer + PITCH_INDEX, convertedValue, 4);
		delete[] convertedValue;
	}

	// Make sure the Roll value is within acceptable range.
	if (moogFrame->roll >= -ROLL_MAX && moogFrame->roll <= ROLL_MAX) {
		// Convert the value from degrees into radians.
		float value = moogFrame->roll;
		value *= (float)(PI / 180.0);

		// Convert the float to network byte order.
		unsigned char *convertedValue = f_nbo_uc(value);

		// Copy the converted value into the command buffer.
		memcpy(m_commandBuffer + ROLL_INDEX, convertedValue, 4);
		delete[] convertedValue;
	}

	/*
		Step2 : (avi , sending these commands to the MBC.)
		This step includes the update for the MBC commands (the commands that would be sent to the MBC in the new version).
		This commands are like human commands , only string and then LOAd and Execute them to the MBC interface.
	*/

	string commandString = "";

	commandString += DOF_LONG_COMMAND + to_string(moogFrame->surge) + "\n";
	commandString += DOF_HEAVE_COMMAND + to_string(moogFrame->heave) + "\n";
	commandString += DOF_LAT_COMMAND + to_string(moogFrame->lateral) + "\n";
	commandString += DOF_ROLL_COMMAND + to_string(moogFrame->roll) + "\n";
	commandString += DOF_PITCH_COMMAND + to_string(moogFrame->pitch) + "\n";
	commandString += DOF_YAW_COMMAND + to_string(moogFrame->yaw) + "\n";

	m_myfile << "new command string";
	m_myfile << commandString;
	m_myfile.flush();

	m_pScriptFile->Load(commandString , m_myfile);
	m_pScriptFile->Execute(&m_MBCIF);


	m_myfile << "end command string\n\n";
	m_myfile.flush();
}

MoogFrame MoogCom::GetAxesPosition()
{
	MoogFrame currentFrame;

	currentFrame.heave = m_MBCIF.GetDofPosition()->heaveFeedback;
	currentFrame.lateral = m_MBCIF.GetDofPosition()->lateralFeedback;
	currentFrame.pitch = m_MBCIF.GetDofPosition()->pitchFeedback;
	currentFrame.roll = m_MBCIF.GetDofPosition()->rollFeedback;
	currentFrame.surge = m_MBCIF.GetDofPosition()->longitudinalFeedback;
	currentFrame.yaw = m_MBCIF.GetDofPosition()->yawFeedback;

	return currentFrame;
}


double MoogCom::ExtractReturnData(int word)
{
	unsigned char arrayCopy[4];

	memcpy(arrayCopy, m_receiveBuffer + word * 4, 4);

	return (double)uc_hbo_f(arrayCopy);
}


void MoogCom::GetAxesPositions(MoogFrame *moogFrame)
{
	/*
		avi : 
		This function is called after getting the current frame position and taking them from the commandBuffer here.
		After that , if there are high transition in the places values , there would be interpolation by the MovePLatform function.
	*/
	unsigned char currentPosition[4];

	// Pull the data for each axis out of the command buffer and convert it from and unsigned
	// char in network-byte-order into a float in host-byte-order.
	EnterCriticalSection(&m_comCS);
	moogFrame->heave = uc_hbo_f((unsigned char*)memcpy(currentPosition, m_commandBuffer + HEAVE_INDEX, 4));
	moogFrame->lateral = uc_hbo_f((unsigned char*)memcpy(currentPosition, m_commandBuffer + LATERAL_INDEX, 4));
	moogFrame->surge = uc_hbo_f((unsigned char*)memcpy(currentPosition, m_commandBuffer + SURGE_INDEX, 4));
	moogFrame->yaw = uc_hbo_f((unsigned char*)memcpy(currentPosition, m_commandBuffer + YAW_INDEX, 4)) / (float)PI*180.0f;
	moogFrame->pitch = uc_hbo_f((unsigned char*)memcpy(currentPosition, m_commandBuffer + PITCH_INDEX, 4)) / (float)PI*180.0f;
	moogFrame->roll = uc_hbo_f((unsigned char*)memcpy(currentPosition, m_commandBuffer + ROLL_INDEX, 4)) / (float)PI*180.0f;
	LeaveCriticalSection(&m_comCS);
}


void MoogCom::ThreadGetAxesPositions(MoogFrame *moogFrame)
{
	unsigned char currentPosition[4];

	// Pull the data for each axis out of the command buffer and convert it from and unsigned
	// char in network-byte-order into a float in host-byte-order.
	moogFrame->heave = uc_hbo_f((unsigned char*)memcpy(currentPosition, m_commandBuffer + HEAVE_INDEX, 4));
	moogFrame->lateral = uc_hbo_f((unsigned char*)memcpy(currentPosition, m_commandBuffer + LATERAL_INDEX, 4));
	moogFrame->surge = uc_hbo_f((unsigned char*)memcpy(currentPosition, m_commandBuffer + SURGE_INDEX, 4));
	moogFrame->yaw = uc_hbo_f((unsigned char*)memcpy(currentPosition, m_commandBuffer + YAW_INDEX, 4)) / (float)PI*180.0f;
	moogFrame->pitch = uc_hbo_f((unsigned char*)memcpy(currentPosition, m_commandBuffer + PITCH_INDEX, 4)) / (float)PI*180.0f;
	moogFrame->roll = uc_hbo_f((unsigned char*)memcpy(currentPosition, m_commandBuffer + ROLL_INDEX, 4)) / (float)PI*180.0f;
}


float MoogCom::GetAxisPosition(Axis axis)
{
	unsigned char currentPosition[4];
	float fcurrentPos;
	bool convert_rad2deg = false;

	EnterCriticalSection(&m_comCS);
	switch (axis)
	{
	case Axis::Heave:
		memcpy(currentPosition, m_commandBuffer + HEAVE_INDEX, 4);
		break;
	case Axis::Lateral:
		memcpy(currentPosition, m_commandBuffer + LATERAL_INDEX, 4);
		break;
	case Axis::Surge:
		memcpy(currentPosition, m_commandBuffer + SURGE_INDEX, 4);
		break;
	case Axis::Yaw:
		memcpy(currentPosition, m_commandBuffer + YAW_INDEX, 4);
		convert_rad2deg = true;
		break;
	case Axis::Pitch:
		memcpy(currentPosition, m_commandBuffer + PITCH_INDEX, 4);
		convert_rad2deg = true;
		break;
	case Axis::Roll:
		memcpy(currentPosition, m_commandBuffer + ROLL_INDEX, 4);
		convert_rad2deg = true;
		break;
	};
	LeaveCriticalSection(&m_comCS);

	fcurrentPos = uc_hbo_f(currentPosition);

	// Convert from radians to degrees if flagged to do so.
	if (convert_rad2deg == true) {
		fcurrentPos *= (float)(180.0 / PI);
	}

	return fcurrentPos;
}


void MoogCom::DoCompute(unsigned char code)
{
	EnterCriticalSection(&m_comCS);
	m_computeCode = code;
	LeaveCriticalSection(&m_comCS);
}


void MoogCom::ThreadDoCompute(unsigned char code)
{
	m_computeCode = code;
}


void MoogCom::SetPacketRate(double packetRate)
{
	m_packetRate = packetRate;
}


double MoogCom::GetPacketRate() const
{
	return m_packetRate;
}


BOOL MoogCom::SetComThreadPriority(int nPriority)
{
	BOOL x = FALSE;

	if (m_talker != NULL) {
		x = SetThreadPriority(m_talker, nPriority);
	}

	return x;
}


int MoogCom::Connect(int &errorCode)
{
	int wasError = 0;

	// Throw up an error if we've already connected.
	if (m_continueSending == false) {
		unsigned char motionCommand = 0x82;

		// Create the talker thread and make it realtime priority.
		m_continueSending = true;
		m_talker = (HANDLE)_beginthread(talker, 0, this);
		if ((uintptr_t)m_talker == -1L) {
			// Thread failed to be created.
			errorCode = errno;
			return -1;
		}
		SetThreadPriority(m_talker, THREAD_PRIORITY_TIME_CRITICAL);

#if !DOF_MODE
		int i;

		motionCommand = 0x80;

		// Put the MBC in MDA mode.
		EnterCriticalSection(&m_comCS);
		for (i = 0; i < PACKET_SIZE; i++) {
			m_commandBuffer[i] = 0x00;
		}
		m_commandBuffer[3] = 0x8C;
		LeaveCriticalSection(&m_comCS);
		//Sleep(2 * (DWORD)ceil(m_packetRate));  // Gives the MBC time to switch into the new mode.
		Sleep(50);
#endif

		// Set the command word to a motion command.
		EnterCriticalSection(&m_comCS);
		m_commandBuffer[3] = motionCommand;
		LeaveCriticalSection(&m_comCS);
	}
	else {
		// Already connected.
		return 1;
	}

	// No errors!
	return 0;
}


void MoogCom::Disconnect()
{
	Park();

	// Tell the communications thread to die.
	m_continueSending = false;

	// Sleeping 50ms allows the stop-sending flag to propogate through the
	// communications thread and gracefully terminate it in the event that
	// the parent process calls this function right before it exits.  Otherwise,
	// you will probably get some sort of memory access violation.
	Sleep(50);

	m_talker = NULL;
}


void MoogCom::Reset()
{
	// Park the motion base before we reset it so that the user doesn't cause
	// it to make sudden movements.
	Park();

	// Get the command buffer lock and stuff the Reset command inside.
	EnterCriticalSection(&m_comCS);
	m_commandBuffer[3] = 0xA0;
	LeaveCriticalSection(&m_comCS);
}

int MoogCom::Engage()
{
	// Make sure we don't call the Engage command when the motion base is already
	// engaged.
	if (m_isEngaged) {
		return -1;	// Already engaged.
	}

	// Make sure that we're connected to the MBC.
	if (m_continueSending == false) {
		return 1;	// Need to connect first.
	}

	// Set the flag that tells us that we've engaged the MOOG.
	m_isEngaged = true;

	EnterCriticalSection(&m_comCS);

	m_myfile << "Engaged by MoogDot!!!!!!!!\n";

	// Make sure the Compute() function is not called.
	m_doCompute = false;

	string s = "Mode Dof\nDOF heave ";
	s += to_string(MOTION_BASE_CENTER);
	s += "\nMDOF long 0";
	s += "\nMDOF yaw 0";
	s += "\nMDOF pitch 0";
	s += "\nMDOF roll 0";
	s += "\nMDOF lat 0";
	s += "\nReset\nWaitForState Ready 60 IgnoreFaults\nEngage\nWaitForState Engaged 30\n";

//	m_pScriptFile->Load("Mode Dof\nDOF heave -0.2207\nReset\nWaitForState Ready 60 IgnoreFaults\nEngage\nWaitForState Engaged 30\n", m_myfile);
	m_pScriptFile->Load(s, m_myfile);


	m_pScriptFile->Execute(&m_MBCIF);


	int i;
	//This update to the commandBuffer and ThreadSetAxisPosition is for updating the currentPositions variable for tracing the positions (and not for commannds).
	// Get the command buffer lock and stuff the Engage command inside.
	for (i = 0; i < PACKET_SIZE; i++) {
		m_commandBuffer[i] = 0x00;
	}

#if DOF_MODE
	// Starts the motion base at the midpoint.
	ThreadSetAxisPosition(Axis::Heave, MOTION_BASE_CENTER);
	m_commandBuffer[3] = 0xAF;
#endif


	/*Sleep(4000);

	string heaveCommand = "DOF Long ";

	float place = 0;
	for (int i = 0; i < 2000; i++)
	{
	Sleep(10);
	place += 0.00001;
	string command = heaveCommand + to_string(place) + "\n";

	m_pScriptFile->Load(command);
	m_pScriptFile->Execute(&m_MBCIF);
	}
	*/

	LeaveCriticalSection(&m_comCS);

	Sleep(50);		// Only send the Engage command a couple times.

	return 0;
}


void MoogCom::Park()
{
	EnterCriticalSection(&m_comCS);

	// Make sure the Compute() function isn't called.
	m_doCompute = false;

	//This update to the commandBuffer and ThreadSetAxisPosition is for updating the currentPositions variable for tracing the positions (and not for commannds).
	// Get the command buffer lock and stuff the Park command inside.
	int i;
	for (i = 0; i < PACKET_SIZE; i++) {
		m_commandBuffer[i] = 0x00;
	}
	m_commandBuffer[3] = 0xD2;

#if DOF_MODE
	m_commandBuffer[3] = 0x82;
#else
	m_commandBuffer[3] = 0x80;
#endif
	
	m_myfile << "Disengaged by MoogDot!!!!!!!!\n";

	//Load the command and execute them in the MBC interface.
	m_pScriptFile->Load("Disengage\nWaitForState Ready 30 IgnoreFaults\n" , m_myfile);
	m_pScriptFile->Execute(&m_MBCIF);


	LeaveCriticalSection(&m_comCS);
	Sleep(50);

	// Flag that the motion base is not engaged now.
	m_isEngaged = false;
}


unsigned char * MoogCom::f_nbo_uc(float num)
{
	unsigned char flipper[4];
	unsigned char *newbuf;

	newbuf = new unsigned char[4];

	memcpy(flipper, &num, 4);
	memcpy(newbuf + 0, flipper + 3, 1);
	memcpy(newbuf + 1, flipper + 2, 1);
	memcpy(newbuf + 2, flipper + 1, 1);
	memcpy(newbuf + 3, flipper + 0, 1);

	return newbuf;
}


float MoogCom::uc_hbo_f(const unsigned char *arg)
{
	float f;
	unsigned char flipper[4];

	memcpy(flipper + 0, arg + 3, 1);
	memcpy(flipper + 1, arg + 2, 1);
	memcpy(flipper + 2, arg + 1, 1);
	memcpy(flipper + 3, arg + 0, 1);
	memcpy(&f, flipper, 4);

	return f;
}
