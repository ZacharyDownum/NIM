// server_main.cpp
//   This function serves as the "main" function for the server-side
#include "../Resources/nim.h"
#include <iostream>
#include <string>
#include <WinSock2.h>

using std::string;

int serverMain(int argc, char *argv[], std::string playerName) {
	SOCKET s;
	char recvBuffer[MAX_RECV_BUFFER];
	char host[v4AddressSize];
	char port[portNumberSize];
	char responseStr[MAX_SEND_BUFFER];

	s = passivesock(NIM_UDPPORT, "udp");

	std::cout << std::endl << "Waiting for a challenge..." << std::endl;
	int len = UDP_recv(s, recvBuffer, MAX_RECV_BUFFER, host, port);
	if (debug) {
		std::cout << timestamp() << " - Received: " << recvBuffer << " from " << host << ":" << port << std::endl;
	}

	bool finished = false;
	while (!finished) {
		if (strcmp(recvBuffer, NIM_QUERY) == 0) {
			// Respond to name query
			strcpy_s(responseStr, NIM_NAME);
			strcat_s(responseStr, playerName.c_str());
			if (debug) {
				std::cout << timestamp() << " - Sending: " << responseStr << " to " << host << ":" << port << std::endl;
			}
			UDP_send(s, responseStr, strlen(responseStr) + 1, host, port);

		} else if (strncmp(recvBuffer, NIM_CHALLENGE, strlen(NIM_CHALLENGE)) == 0) {
			// Received a challenge  
			char *startOfName = strstr(recvBuffer, NIM_CHALLENGE);
			if (startOfName != NULL) {
				std::cout << std::endl << "You have been challenged by " << startOfName + strlen(NIM_CHALLENGE) << std::endl;
			}

			string response;
			std::getline(std::cin, response);

			if (response[0] == 'y' || response[0] == 'Y') {

				strcpy_s(responseStr, NIM_CONFIRM);
				UDP_send(s, responseStr, strlen(responseStr) + 1, host, port);

				if (debug) {

					std::cout << timestamp() << " - Sent: " << recvBuffer << " to " << host << ":" << port << std::endl;
				}

				int status = wait(s, 2, 0);

				if (status > 0) {

					len = UDP_recv(s, recvBuffer, MAX_RECV_BUFFER, host, port);

					if (debug) {

						std::cout << timestamp() << " - Received: " << recvBuffer << " from " << host << ":" << port << std::endl;
					}

					char *confirmation = strstr(recvBuffer, NIM_CONFIRM_HANDSHAKE);

					if (confirmation != nullptr) {

						// Play the game.  You are the player two
						int winner = playNim(s, (char*)playerName.c_str(), host, port, PLAYER_TWO);
						finished = true;
					}
				} else {

					if (debug) {

						std::cout << timestamp() << " Failed to recieve a response from " << host << ":" << port << std::endl;
					}
				}
			} else {

				strcpy_s(responseStr, NIM_DECLINE);
				UDP_send(s, responseStr, strlen(responseStr) + 1, host, port);

				if (debug) {

					std::cout << timestamp() << " - Sent: " << recvBuffer << " to " << host << ":" << port << std::endl;
				}
			}
		}

		if (!finished) {
			char previousBuffer[MAX_RECV_BUFFER];	strcpy_s(previousBuffer, recvBuffer);
			char previousHost[v4AddressSize];				strcpy_s(previousHost, host);
			char previousPort[portNumberSize];				strcpy_s(previousPort, port);

			// Check for duplicate datagrams (can happen if broadcast enters from multiple ethernet connections)
			bool newDatagram = false;
			int status = wait(s, 1, 0);	// We'll wait a second to see if we receive another datagram
			while (!newDatagram && status > 0) {
				len = UDP_recv(s, recvBuffer, MAX_RECV_BUFFER, host, port);
				if (debug) {
					std::cout << timestamp() << " - Received: " << recvBuffer << " from " << host << ":" << port << std::endl;
				}
				if (strcmp(recvBuffer, previousBuffer) == 0 &&		// If this datagram is identical to previous one
					strcmp(host, previousHost) == 0 &&		//  and it came from a previously known host
					strcmp(port, previousPort) == 0) {		//  using the same port number, then ignore it.
					status = wait(s, 1, 0);			// Wait another second (still more copies?)
				} else {
					newDatagram = true;		// if not identical to previous one, keep it!
				}
			}

			// If we waited one (or more seconds) and received no new datagrams, wait for one now.
			if (!newDatagram) {
				len = UDP_recv(s, recvBuffer, MAX_RECV_BUFFER, host, port);
				if (debug) {
					std::cout << timestamp() << " - Received: " << recvBuffer << " from " << host << ":" << port << std::endl;
				}
			}
		}
	}
	closesocket(s);

	return 0;
}