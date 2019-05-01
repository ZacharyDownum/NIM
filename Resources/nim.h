// nim.h    Header file for Nim project
#pragma once

#include <winsock2.h>
#include <string>

#define debug true
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

const char NIM_CONFIRM[] = "YES";
const char NIM_DECLINE[] = "NO";
const char NIM_CONFIRM_HANDSHAKE[] = "GREAT!";

const int NO_WINNER = 0;
const int PLAYER_SERVER = 1;
const int PLAYER_CLIENT = 2;
const int LOCAL_FORFEIT = 3;
const int REMOTE_FORFEIT = 4;
const int ABORT = 5;

struct ServerStruct {
	std::string name;
	std::string host;
	std::string port;
};

// TODO: Finalize the parameter list for playNim and update declaration
int playNim(SOCKET s, std::string serverName, std::string remoteIP, std::string remotePort, int localPlayer);

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