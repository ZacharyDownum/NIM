// NIMClient.cpp
//   This function serves as the "main" function for the client-side
#include "nim.h"
#include <iostream>
#include <string>
#include <WinSock2.h>

int clientMain(int argc, char *argv[], std::string playerName)
{
	char clientName[MAX_NAME];

	std::cout << "What is your name? ";
	std::getline(std::cin, clientName);

	// Find all NIM servers on our subnet
	SOCKET s = connectsock("","","udp");	// Create a UDP socket  (Don't need to designate a host or port for UDP)
	std::cout << std::endl << "Looking for NIM servers ... " << std::endl;

	char broadcastAddress[v4AddressSize];
	char myIPAddress[v4AddressSize];
	int status = getIPAddressInfo(myIPAddress, broadcastAddress);
	if (status != 0)
	{
		std::cerr << "No interface found that supports broadcasting." << std::endl;
		return -1;
	}

	// Build an array of NIM servers found on our subnet.
	ServerStruct serverArray[MAX_SERVERS];
	int numServers = getServers(s, broadcastAddress, NIM_UDPPORT, serverArray);

	if (numServers == 0)
	{
		std::cout << std::endl << "Sorry.  No NIM servers were found.  Try again later." << std::endl << std::endl;
	}
	else
	{
		// Display the list of NIM servers found
		std::cout << std::endl << "Found NIM server";
		if (numServers == 1)
		{
			std::cout << ":" << "  " << serverArray[0].name << std::endl;
		}
		else
		{
			std::cout << "s:" << std::endl;
			printServers(serverArray, numServers);
		}
		std::cout << std::endl;

		// Allow user to select a NIM server to challenge to a game
		int answer = 0;
		std::string answerStr;
		if (numServers == 1)
		{
			std::cout << "Do you want to challenge " << serverArray[0].name << "? ";
			std::getline(std::cin, answerStr);
			if (answerStr[0] == 'y' || answerStr[0] == 'Y')
			{
				answer = 1;
			}
		}
		else if (numServers > 1)
		{
			std::cout << "Who would you like to challenge (1-" << numServers + 1 << ")? ";
			std::getline(std::cin,answerStr);
			answer = atoi(answerStr.c_str());
			if (answer > numServers)
			{
				answer = 0;
			}
		}
			
		while (answer >= 1 && answer <= numServers)
		{
			// Extract the opponent's info from the server[] array
			std::string serverName;
			char host[v4AddressSize];
			char port[portNumberSize];
			serverName = serverArray[answer - 1].name;		// Adjust for 0-based array
			strcpy_s(host, serverArray[answer - 1].host.c_str());
			strcpy_s(port, serverArray[answer - 1].port.c_str());

			// Append playerName to the NIM_CHALLENGE string & send a challenge to host:port
			char buffer[MAX_SEND_BUFFER];
			strcpy_s(buffer, NIM_CHALLENGE);
			strcat_s(buffer, playerName.c_str());
			int len = UDP_send(s, buffer, strlen(buffer) + 1, host, port);
			if (debug) {
				std::cout << timestamp() << " - Sent: " << buffer << " to " << host << ":" << port << std::endl;
			}

			int status = wait(s, 10, 0);
			
			char recvBuffer[MAX_RECV_BUFFER];

			int numberOfBytesReceived = UDP_recv(s, recvBuffer, MAX_RECV_BUFFER, host, port);

			if (status > 0 && numberOfBytesReceived > 0 && stricmp(recvBuffer, NIM_CONFIRM) == 0)
			{
				strcpy_s(buffer, NIM_CONFIRM_HANDSHAKE);
				int len = UDP_send(s, buffer, strlen(buffer) + 1, host, port);
				if (debug) {
					std::cout << timestamp() << " - Sent: " << buffer << " to " << host << ":" << port << std::endl;
				}

				// Play the game.  You are the 'X' player
				int winner = playNim(s, serverName, host, port, PLAYER_X);
			}
			else
			{
				std::cout << "Connection to " << host << ":" << port << " was refused." << std::endl;
				std::cout << "Please select a different server to challenge, or quit:" << std::endl;

				int numServers = getServers(s, broadcastAddress, NIM_UDPPORT, serverArray);
				
				if (numServers == 0)
				{
					std::cout << std::endl << "Sorry.  No NIM servers were found.  Try again later." << std::endl << std::endl;
				}
				else
				{
					std::cout << std::endl << "Found NIM server";
					if (numServers == 1)
					{
						std::cout << ":" << "  " << serverArray[0].name << std::endl;
					}
					else
					{
						std::cout << "s:" << std::endl;
						printServers(serverArray, numServers);
					}
					std::cout << std::endl;

					answer = 0;
					std::string answerStr;
					if (numServers == 1)
					{
						std::cout << "Do you want to challenge " << serverArray[0].name << "? ";
						std::getline(std::cin, answerStr);
						if (answerStr[0] == 'y' || answerStr[0] == 'Y')
						{
							answer = 1;
						}
					}
					else if (numServers > 1)
					{
						std::cout << "Who would you like to challenge (1-" << numServers + 1 << ")? ";
						std::getline(std::cin, answerStr);
						answer = atoi(answerStr.c_str());
						if (answer > numServers)
						{
							answer = 0;
						}
					}
				}
			}
		}
	}

	closesocket(s);
	return 0;
}

void printServers(ServerStruct serverArray[], int numberOfServers)
{
	for (int i = 0; i < numberOfServers; i++) {
		std::cout << "  " << i + 1 << " - " << serverArray[i].name << std::endl;
	}
	std::cout << std::endl << "  " << numberOfServers + 1 << " - QUIT" << std::endl;
}