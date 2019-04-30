// playTicTacToe.cpp
// This set of functions are used to actually play the game.
// Play starts with the function: playTicTacToe() which is defined below

#include "../Resources/nim.h"
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <time.h>
#include <cstdlib>

using std::cout;
using std::endl;

int initializeBoard(int board[])
{
	int result = 0;

	char choiceStr[80], newline;
	int choice = 0;

	do {

		std::cout << "Choose an option:" << std::endl;
		std::cout << "   1 - Generate random piles" << std::endl;
		std::cout << "   2 - Manually generate piles" << std::endl;
		std::cout << "Enter 1 or 2: ";

		std::cin >> choiceStr; std::cin.get(newline);
		choice = atoi(choiceStr);

		switch (choice) {

			case 1:
				
				std::srand(time(0));

				result = (rand() % 7) + 3;

				for (int i = 0; i < result; i++) {

					board[i] = (rand() % 20) + 1;
				}

				break;
			case 2:
				
				std::cout << "How many Piles do you want?" << std::endl;
				std::cin >> result;

				while (result < 3 || result > 9) {
					std::cout << "Invalid Number of Piles. Enter new value in between 3 and 9." << endl;
					std::cin >> result;
				}

				for (int i = 0; i < result; i++) {
					int numberOfRocks = 0;
					std::cout << "Number of rocks in pile" << i + 1 << "?" << endl;
					std::cin >> numberOfRocks;
					while (numberOfRocks < 1 || numberOfRocks > 20) {
						std::cout << "Invalid Number of Rocks. Enter new value in between 1 and 20." << endl;
						std::cin >> numberOfRocks;
					}

					board[i] = numberOfRocks;
				}

				break;
			default:
				
				std::cout << std::endl << "Please enter a digit between 1 and 2." << std::endl;
		}
	} while (choice != 1 && choice != 2);

	return result;
}

void updateBoard( int board[], int move, int player)
{
	if (move < 0 || move > 9) {
		std::cout << "Problem with updateBoard function!" << std::endl;
	}
	else {
		if (player == PLAYER_ONE) {
			board[move] = 'X';
		}
		else if (player == PLAYER_TWO) {
			board[move] = 'O';
		}
		else
			std::cout << "Problem with updateBoard function!" << std::endl;
	}
}

void displayBoard(int board[], int pileCount)
{

	for (int i = 0; i < pileCount; i++)
	{
		int rocksInPile = board[i];
		for (int i = 0; i < rocksInPile; i++)
		{
			std::cout << "*";
		}
		std::cout << endl;
	}
	
}

int check4Win(int board[])
{
	int winner = noWinner;

	if (winner == noWinner && numMoves == 9)
		winner = TIE;
	

	return winner;
}

int getLocalUserMove(SOCKET s, int board[10], int player, std::string remoteIP, std::string remotePort) //unfinished -- forfeit currently unhandled
{
	int move;
	char move_str[80];
	char newMove_str[80];

	std::cout << "Where do you want to place your rock?";

	while (move_str[0] == 'C')
	{
		if (move_str[0] == 'C')
		{
			for (int i = 1; i < strlen(move_str); i++) // takes off 'C' character
			{
				newMove_str[i - 1] = move_str[i];
			}

			UDP_send(s, move_str, strlen(move_str) + 1, remoteIP.c_str(), remotePort.c_str()); //displays comment to opponent

			for (int i = 0; i < strlen(newMove_str); i++)// displays comment to user
			{
				std::cout << newMove_str[i];
			}
			cout << std::endl;
		}
	}

	if (move_str[0] != 'F')
	{
		do {
			std::cout << "Your move? ";
			std::cin >> move_str; //mnn

			move = atoi(move_str);
			if (move[0] < 1 || move[0] > 9) std::cout << "Invalid move.  Try again." << std::endl;
			else {
				if (board[move] == 'X' || board[move] == 'O') {
					move = 0;
					std::cout << "I'm sorry, but that square is already occupied." << std::endl;
				}
			}
		} while (move[0] < 1 || move[0] > 9);
	}

	return move;
}

void sendBoard(SOCKET s, std::string remoteIP, std::string remotePort, int board[], int pileCount) {

	char moveString[MAX_SEND_BUFFER];

	_itoa_s(pileCount, moveString, 10);

	for (int i = 0; i < pileCount; i++) {

		if (i < 10) {

			// Start single digit pile sizes with 0
			_itoa_s(0, moveString, 10);
		}

		_itoa_s(board[i], moveString, 10);
	}

	UDP_send(s, moveString, strlen(moveString) + 1, remoteIP.c_str(), remotePort.c_str());
}

bool receiveBoard(SOCKET s, int board[], int &pileCount) {

	bool recievedBoardSuccessfully = false;

	char boardData[MAX_RECV_BUFFER];
	char host[v4AddressSize];
	char port[portNumberSize];

	int status = UDP_recv(s, boardData, MAX_RECV_BUFFER - 1, host, port);

	if (status > 0) {

		pileCount = std::atoi((const char*)boardData[0]);

		if (pileCount >= 3 && pileCount <= 9) {

			recievedBoardSuccessfully = true;

			for (int i = 1; i < pileCount; i += 2) {

				std::string move;
				move += boardData[i];
				move += boardData[i + 1];

				board[i] = std::stoi(move.c_str(), 0, 10);

				if (board[i] < 0 || board[i] > 20) {

					recievedBoardSuccessfully = false;
					break;
				}
			}
		}
	}

	return recievedBoardSuccessfully;
}

int playNim(SOCKET s, std::string serverName, std::string remoteIP, std::string remotePort, int localPlayer)
{
	// This function plays the game and returns the value: winner.  This value 
	// will be one of the following values: noWinner, xWinner, oWinner, TIE, ABORT
	int winner = noWinner;
	int board[10];
	int opponent;
	int move;
	bool myMove;

	bool initializedBoard = false;

	int pileCount = 0;

	if (localPlayer == PLAYER_SERVER) {

		std::cout << "Playing as server" << std::endl;
		opponent = PLAYER_CLIENT;
		pileCount = initializeBoard(board);
		sendBoard(s, remoteIP, remotePort, board, pileCount);
		initializedBoard = true;
		myMove = false;
	} else {

		std::cout << "Playing as client" << std::endl;
		opponent = PLAYER_SERVER;

		int status = wait(s, 2, 0);

		if (status > 0) {

			initializedBoard = receiveBoard(s, board, pileCount);
		}

		myMove = true;
	}

	if (initializedBoard = true) {

		displayBoard(board, pileCount);

		while (winner == noWinner) {
			if (myMove) {
				// Get my move & display board
				move = getLocalUserMove(s, board, localPlayer, remoteIP, remotePort);
				std::cout << "Board after your move:" << std::endl;
				updateBoard(board,move,localPlayer);
				displayBoard(board, pileCount);

				// Send move to opponent
				char moveString[MAX_SEND_BUFFER];
				_itoa_s(move, moveString, 10);
				UDP_send(s, moveString, strlen(moveString) + 1, remoteIP.c_str(), remotePort.c_str());

			} else {
				std::cout << "Waiting for your opponent's move..." << std::endl << std::endl;
				//Get opponent's move & display resulting board
				int status = wait(s,WAIT_TIME,0);
				if (status > 0) {
					char moveString[MAX_RECV_BUFFER];
					char host[v4AddressSize];
					char port[portNumberSize];
					UDP_recv(s, moveString, MAX_RECV_BUFFER - 1, host, port);
				} 
				else {
					winner = ABORT;
				}
			}
			myMove = !myMove;	// Switch whose move it is

			if (winner == ABORT) {
				std::cout << timestamp() << " - No response from opponent.  Aborting the game..." << std::endl;
			} else {
				winner = check4Win(board);
			}
		
			if (winner == localPlayer)
				std::cout << "You WIN!" << std::endl;
			else if (winner == TIE)
				std::cout << "It's a tie." << std::endl;
			else if (winner == opponent)
				std::cout << "I'm sorry.  You lost" << std::endl;
		}
	} else {

		cout << "Failed to initialize the game board" << endl;
	}

	return winner;
}