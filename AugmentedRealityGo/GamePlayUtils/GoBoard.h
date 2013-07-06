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
	GoBoard(FuegoAssistant* f);
	//void clearBoard();
	bool checkNewBoardState(char newRealBoardStones[361], char newMoveColor);
	void changeTurn();
	int getMoveTurnColor();
    void addVirtualStone(std::string move, std::string color);
	void addRealStone(int stone_index, int color);
	std::string getNewMove();
	void clear_board();
	volatile char realStones[361];
	volatile char virtualStones[361];
	volatile char wrongRealStones[361];
	volatile int newMoveIndex;
	static bool newMoveIsMade;
	static FuegoAssistant* fuego;
private:
	int currentMoveColor;
	
	
};
	
#endif