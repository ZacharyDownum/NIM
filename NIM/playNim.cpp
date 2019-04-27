// playTicTacToe.cpp
// This set of functions are used to actually play the game.
// Play starts with the function: playTicTacToe() which is defined below

#include "TicTacToe.h"
#include <WinSock2.h>
#include <iostream>
#include <string>


void initializeBoard( char board[19], )
{
	char initBoard[10] = {'0','1','2','3','4','5','6','7','8','9'};
	for (int i=0; i<10; i++)
		board[i] = initBoard[i];
}

void updateBoard( char board[19], int move, int player)
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

void displayBoard( char board[19] )
{
	/*std::cout << std::endl;
	std::cout << board[7] << " | " << board[8] << " | " << board[9] << std::endl;
	std::cout << "__+___+__" << std::endl;
	std::cout << board[4] << " | " << board[5] << " | " << board[6] << std::endl;
	std::cout << "__+___+__" << std::endl;
	std::cout << board[1] << " | " << board[2] << " | " << board[3] << std::endl;
	std::cout << std::endl;*/

	
}

int check4Win(char board[10])
{
	int winner = noWinner;

	//// Check for vertical winners
	//int i = 1;
	//while (winner == noWinner && i < 4) {
	//	if (board[i] == board[i+3] && board[i] == board[i+6]) {
	//		winner = (board[i] == 'X') ? winnerIsX : winnerIsO;
	//	}
	//	i++;
	//}

	//// Check for horizontal winners
	//i = 1;
	//while (winner == noWinner && i < 8) {
	//	if (board[i] == board[i+1] && board[i] == board[i+2]) {
	//		winner = (board[i] == 'X') ? winnerIsX : winnerIsO;
	//	}
	//	i  += 3;
	//}

	//// Check for diagonal winners
	//if (winner == noWinner) {
	//	if ( (board[1] == board[5] && board[1] == board[9]) ||
	//		 (board[3] == board[5] && board[3] == board[7]) ) {
	//		winner = (board[5] == 'X') ? winnerIsX : winnerIsO;
	//	}
	//}

	//// Check for tie
	//i = 1;
	//int numMoves = 0;
	//while ( i < 10) {
	//	if ( (board[i] == 'X' || board[i] == 'O') ) {
	//		numMoves++;
	//	}
	//	i++;
	//}

	if (winner == noWinner && numMoves == 9)
		winner = TIE;
	

	return winner;
}

int getLocalUserMove(SOCKET s, char board[10], int player, std::string remoteIP, std::string remotePort)
{
	int move;
	char move_str[80];
	char newMove_str[80];

	std::cout << "Where do you want to place your rock?";
	//char mark = (player == PLAYER_ONE) ? 'X' : 'O';
	//std::cout << mark << "? " << std::endl;

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

	return move;
}

int playTicTacToe(SOCKET s, std::string serverName, std::string remoteIP, std::string remotePort, int localPlayer)
{
	// This function plays the game and returns the value: winner.  This value 
	// will be one of the following values: noWinner, xWinner, oWinner, TIE, ABORT
	int winner = noWinner;
	char board[10];
	int opponent;
	int move;
	bool myMove;

	if (localPlayer == PLAYER_ONE) {
		std::cout << "Playing as Player One" << std::endl;
		opponent = PLAYER_TWO;
		myMove = true;
	} else {
		std::cout << "Playing as Player Two" << std::endl;
		opponent = PLAYER_ONE;
		myMove = false;
	}

	initializeBoard(board);
	displayBoard(board);

	while (winner == noWinner) {
		if (myMove) {
			// Get my move & display board
			move = getLocalUserMove(s, board, localPlayer, remoteIP, remotePort);
			std::cout << "Board after your move:" << std::endl;
			updateBoard(board,move,localPlayer);
			displayBoard(board);

			// Send move to opponent
/****			
	Task 1: "move" is an integer that was assigned a value (from 1 to 9) in the previous code segment.
	         Add code here to convert "move" to a null-terminated C-string and send it to your opponent at remoteIP using remotePort.
****/
            char moveString[MAX_SEND_BUFFER];
            _itoa_s(move, moveString, 10);
            UDP_send(s, moveString, strlen(moveString) + 1, remoteIP.c_str(), remotePort.c_str());

		} else {
			std::cout << "Waiting for your opponent's move..." << std::endl << std::endl;
			//Get opponent's move & display resulting board
			int status = wait(s,WAIT_TIME,0);
			if (status > 0) {
/****			
Task 2: (i) Insert code inside this IF statement that will accept a null-terminated C-string from your
		opponent that represents their move.  Convert that string to an integer and then
		(ii) call a function that will update the game board (see above) using your opponent's move, and
		(iii) call a function that will display the updated board on your screen.
****/       
            char moveString[MAX_RECV_BUFFER];
            char host[v4AddressSize];
            char port[portNumberSize];
            UDP_recv(s, moveString, MAX_RECV_BUFFER - 1, host, port);

            

			} else {
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

	return winner;
}