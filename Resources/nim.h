// nim.h    Header file for Nim project
#pragma once

#include <winsock2.h>
#include <string>

#define NIM_QUERY	"Who?"
#define NIM_NAME	"Name="
#define NIM_CHALLENGE	"Player="

static char NIM_UDPPORT[] = "29333";

const int v4AddressSize = 16;
const int portNumberSize = 10;
const int WAIT_TIME = 120;
const int MAX_SEND_BUFFER  = 2048;
const int MAX_RECV_BUFFER  = 2048;
const int MAX_SERVERS   = 100;
const int MAX_NAME = 80;

struct ServerStruct {
	std::string name;
	std::string host;
	std::string port;
};

// TODO: Finalize the parameter list for playNim and update declaration
void playNim(SOCKET s, bool, char*, bool, bool, char*, char*, bool);

SOCKET connectsock (const char *host, const char *service, const char *protocol);
SOCKET passivesock (const char *service, const char *protocol);
int UDP_recv (SOCKET s, char *buf, int maxSize, char *remoteHost, char *remotePort);
int UDP_send (SOCKET s, const char *buf, int numBytesToSend, const char *host, const char *service);
int wait(SOCKET s, int seconds, int msec);
char* timestamp();
int getServers(SOCKET s, const char *broadcastAddress, const char *remotePort, ServerStruct serverArray[]);
int serverMain(int argc, char *argv[], std::string playerName);
int clientMain(int argc, char *argv[], std::string playerName);
int getIPAddressInfo(char *myIPAddress, char *myBroadcastAddress);