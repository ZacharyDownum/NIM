// playTicTacToe.cpp
// This set of functions are used to actually play the game.
// Play starts with the function: playTicTacToe() which is defined below

#include "../Resources/nim.h"
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <time.h>
#include <cstdlib>
#include <iomanip>
#include <cctype>

using std::cout;
using std::cin;
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
		/*if (player == PLAYER_ONE) {
			board[move] = 'X';
		}
		else if (player == PLAYER_TWO) {
			board[move] = 'O';
		}
		else
			std::cout << "Problem with updateBoard function!" << std::endl;*/
	}
}

void displayBoard(int board[], int pileCount)
{
	cout << "\n\nNim board:" << endl;
	cout << std::setfill('-') << std::setw(80) << "-" << endl;

	for (int i = 0; i < pileCount; i++)
	{
		int rocksInPile = board[i];

		cout << "Rock Pile #" << i + 1 << " -> ";

		for (int i = 0; i < rocksInPile; i++)
		{
			std::cout << "* ";
		}
		std::cout << endl;
	}

	cout << std::setfill('-') << std::setw(80) << "-" << endl;
}

int check4Win(int board[])
{
	int winner = NO_WINNER;
	

	return winner;
}

void sendChat(SOCKET s, std::string remoteIP, std::string remotePort) {

	std::string message;

	cout << "Comment? ";

	do {

		std::getline(cin, message);
	} while (message.length() == 0);

	if (message.length() > 0) {

		std::string sendBuffer = "C" + message;

		UDP_send(s, sendBuffer.c_str(), sendBuffer.length() + 1, remoteIP.c_str(), remotePort.c_str()); //displays comment to opponent

		if (debug) {

			std::cout << timestamp() << " - Sent: " << sendBuffer << " to " << remoteIP << ":" << remotePort << std::endl;
		}
	}
}

void getLocalUserMove(SOCKET s, std::string remoteIP, std::string remotePort, int board[10], int pileCount, int &oSelectedPile, int &oRemovedRockCount) {

	bool madeValidMove = false;

	do {

		std::cout << "From which pile would you like to remove some rocks (1-" << pileCount << ")? ";
		std::cin >> oSelectedPile; //mnn

		// Validate selected pile
		if (oSelectedPile >= 1 || oSelectedPile <= pileCount) {

			if (board[oSelectedPile - 1] > 0) {

				cout << "How many rocks would you like to remove from pile #" << oSelectedPile << " (1-" << board[oSelectedPile - 1] << ")? ";

				cin >> oRemovedRockCount;

				// Validate selected rock count
				if (oRemovedRockCount >= 1 || oRemovedRockCount <= board[oSelectedPile - 1]) {

					board[oSelectedPile - 1] -= oRemovedRockCount;
					madeValidMove = true;
				} else {
					cout << "You can't remove " << oRemovedRockCount << " rocks from pile #" << oSelectedPile << "!" << endl;
				}
			} else {

				cout << "There are no rocks in pile #" << oSelectedPile << "!  Please try again." << endl;
			}
		} else {

			std::cout << "Invalid move.  Try again." << std::endl;
		}
	} while (madeValidMove == false);
}

void sendBoard(SOCKET s, std::string remoteIP, std::string remotePort, int board[], int pileCount) {

	char boardString[MAX_SEND_BUFFER];

	strcpy_s(boardString, std::to_string(pileCount).c_str());

	for (int i = 0; i < pileCount; i++) {

		if (board[i] < 10) {

			// Start single digit pile sizes with 0
			strcat_s(boardString, std::to_string(0).c_str());
		}

		strcat_s(boardString, std::to_string(board[i]).c_str());
	}

	UDP_send(s, boardString, strlen(boardString) + 1, remoteIP.c_str(), remotePort.c_str());

	if (debug) {

		std::cout << timestamp() << " - Sent: " << boardString << " to " << remoteIP << ":" << remotePort << std::endl;
	}
}

bool receivedBoard(SOCKET s, int board[], int &pileCount) {

	bool recievedBoardSuccessfully = false;

	char recvBuffer[MAX_RECV_BUFFER];
	char host[v4AddressSize];
	char port[portNumberSize];

	int length = UDP_recv(s, recvBuffer, MAX_RECV_BUFFER - 1, host, port);

	if (debug) {

		std::cout << timestamp() << " - Recieved: " << recvBuffer << " from " << host << ":" << port << std::endl;
	}

	if (length > 0) {

		pileCount = recvBuffer[0] - '0';

		if (pileCount >= 3 && pileCount <= 9 && length >= pileCount * 2 + 1) {

			recievedBoardSuccessfully = true;

			int currentPile = 0;

			for (int i = 1; i < pileCount * 2 + 1; i += 2) {

				std::string move;
				move += recvBuffer[i];
				move += recvBuffer[i + 1];

				board[currentPile] = std::stoi(move.c_str(), 0, 10);

				if (board[currentPile] < 1 || board[currentPile] > 20) {

					recievedBoardSuccessfully = false;
					break;
				}

				currentPile++;
			}
		}
	}

	return recievedBoardSuccessfully;
}

int playNim(SOCKET s, std::string serverName, std::string remoteIP, std::string remotePort, int localPlayer)
{
	// This function plays the game and returns the value: winner.  This value 
	// will be one of the following values: noWinner, xWinner, oWinner, TIE, ABORT
	int winner = NO_WINNER;
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

			initializedBoard = receivedBoard(s, board, pileCount);
		}

		myMove = true;
	}

	if (initializedBoard == true) {

		displayBoard(board, pileCount);

		while (winner == NO_WINNER) {
			if (myMove) {

				int selectedPile, removedRocks;

				char choice;
				bool madeMove = false;
				bool forfeited = false;

				cout << "Your turn." << endl;

				do {

					cout << "Enter first letter of one of the following commands (C,F,H, or R)." << endl;
					cout << "Command (Chat, Forfeit, Help, Remove-rocks)? ";
					cin >> choice;

					switch (choice) {

						case 'c':
						case 'C':

							sendChat(s, remoteIP, remotePort);
							break;
						case 'r':
						case 'R':

							getLocalUserMove(s, remoteIP, remotePort, board, pileCount, selectedPile, removedRocks);
							madeMove = true;
							break;
						case 'f':
						case 'F':

							cout << "Do you want to forfeit this game (Y/N)? ";

							cin >> choice;

							if (choice == 'y' || choice == 'Y') {

								forfeited = true;
							}

							winner = LOCAL_FORFEIT;
							break;
					}
				} while (!madeMove && !forfeited);

				if (madeMove) {

					std::cout << "Board after your move:" << std::endl;
					//updateBoard(board,move,localPlayer);
					displayBoard(board, pileCount);

					// Send move to opponent
					char moveString[MAX_SEND_BUFFER];
					strcpy_s(moveString, std::to_string(selectedPile).c_str());

					if (removedRocks < 10) {

						strcat_s(moveString, std::to_string(0).c_str());
					}

					strcat_s(moveString, std::to_string(removedRocks).c_str());

					UDP_send(s, moveString, strlen(moveString) + 1, remoteIP.c_str(), remotePort.c_str());

					if (debug) {

						std::cout << timestamp() << " - Sent: " << moveString << " to " << remoteIP << ":" << remotePort << std::endl;
					}
				}
			} else {
				std::cout << "Waiting for your opponent's move..." << std::endl << std::endl;
				//Get opponent's move & display resulting board
				int status = wait(s,WAIT_TIME,0);
				if (status > 0) {
					char moveString[MAX_RECV_BUFFER];
					char host[v4AddressSize];
					char port[portNumberSize];
					int length = UDP_recv(s, moveString, MAX_RECV_BUFFER - 1, host, port);

					if (debug) {

						std::cout << timestamp() << " - Recieved: " << moveString << " from " << host << ":" << port << std::endl;
					}

					if (length > 0) {

						moveString[length] = '\n';

						if (moveString[0] == 'c' || moveString[0] == 'C') {

							std::string message = moveString;

							message = message.substr(1, message.length() - 1);

							cout << "\n(CHAT MESSAGE from " << serverName << "):" << endl;
							cout << "   " << message << endl;
						} else if (std::isdigit(moveString[0])) {

							std::string move = moveString;

							int pile = move[0] - '0';
							int removedRocks = std::stoi(move.substr(1, move.length() - 1));

							if (pile < 1 || pile > pileCount || removedRocks < 1 || removedRocks > board[pile - 1]) {

								cout << "Opponent entered an invalid move. You win by default!" << endl;
								winner = DEFAULT_WIN;
							} else {

								board[pile - 1] -= removedRocks;
								displayBoard(board, pileCount);
							}
						} else if (moveString[0] == 'f' || moveString[0] == 'F') {

							winner = REMOTE_FORFEIT;
						}
					} else {
						winner = ABORT;
					}
				} 
				else {
					winner = ABORT;
				}
			}

			if (winner == ABORT) {
				std::cout << timestamp() << " - No response from opponent.  Aborting the game..." << std::endl;
			} else if (winner == LOCAL_FORFEIT) {

				std::string forfeitString = "F";

				UDP_send(s, forfeitString.c_str(), forfeitString.length() + 1, remoteIP.c_str(), remotePort.c_str());
			} else if (winner == REMOTE_FORFEIT) {

				cout << serverName << " has forfeited!  YOU WIN!!" << endl;
			} else {

				bool noRocksRemain = true;

				// Check for win conditions
				for (int i = 0; i < pileCount; i++) {

					if (board[i] > 0) {

						noRocksRemain = false;
					}
				}

				if (noRocksRemain) {

					if (myMove) {

						winner = localPlayer;
					} else {

						winner = opponent;
					}
				}
			}
		
			if (winner == localPlayer)
				std::cout << "You WIN!" << std::endl;
			else if (winner == opponent)
				std::cout << "I'm sorry.  You lost" << std::endl;

			myMove = !myMove;	// Switch whose move it is
		}
	} else {

		cout << "Failed to initialize the game board" << endl;
	}

	return winner;
}