#ifndef FuegoAssistant_H
#define FuegoAssistant_H

////////////////////////////////////////////////////////////////////
#include <string>
#include <sstream>
#include <vector>
#include <iostream> // for standard I/O
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/process.hpp>
#include "Helper.h"
#include "GameConstant.h"
/*
	wrapper to communicate with fuego 
*/



class FuegoAssistant
{
public:
	FuegoAssistant();
	~FuegoAssistant();
	bool getBookPositions();
	void clear_board();
	void genMove(std::string color);
	static std::vector<int> bookMoves;
	void addMove(std::string move, int color);
	void showBoard();
private:
	static boost::process::child c;
	
	
};
	
#endif