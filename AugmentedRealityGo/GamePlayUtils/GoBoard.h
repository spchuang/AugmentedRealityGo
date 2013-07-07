#ifndef GoBoard_H
#define GoBoard_H

////////////////////////////////////////////////////////////////////
#include <string>
#include <sstream>
#include <vector>
#include <iostream> // for standard I/O
/*
	Representation of Go Board
*/
#include "GameConstant.h"
#include "Helper.h"
#include "FuegoAssistant.h"
class GoBoard
{
public:
	GoBoard();
	//void clearBoard();
	void setFuego(FuegoAssistant* f);
	bool checkNewBoardState(char newRealBoardStones[361], char newMoveColor);
	
	
	//clear board
	void clear_board();

	//add stones
	void changeTurn();
    void addVirtualStone(std::string move, std::string color);
	void addRealStone(int stone_index, int color);

	//return the color of current turn
	int getMoveTurnColor();

	std::string getNewMove();
	
	volatile char realStones[361];
	volatile char virtualStones[361];
	volatile char wrongRealStones[361];
	volatile int newMoveIndex;
	static bool newMoveIsMade;
	
private:
	int currentMoveColor;
	FuegoAssistant* fuego;
	
};
	
#endif