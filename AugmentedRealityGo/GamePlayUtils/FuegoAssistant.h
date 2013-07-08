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
	bool estimateTerritory(int color);
	void clear_board();
	void genMove(std::string color);
	
	bool addMove(std::string move, int color);
	void showBoard();
	bool sendCommandWithEmptyResponse(std::string command, std::string& readLine);

	static std::vector<int> bookMoves;
	static std::vector<float> estimateScore;
	
private:

	static boost::process::child c;
	bool setTerritoryParam;
	bool bookLoaded;
	
};
	
#endif