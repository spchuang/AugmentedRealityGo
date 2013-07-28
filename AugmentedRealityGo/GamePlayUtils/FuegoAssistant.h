#ifndef FuegoAssistant_H
#define FuegoAssistant_H

////////////////////////////////////////////////////////////////////
#include <string>
#include <sstream>
#include <boost/thread/thread.hpp>
#include <vector>
#include <iostream> // for standard I/O
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/process.hpp>
#include "../HelperClass/Helper.h"
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
	void getFuegoMove(int color);
	void set_memory_limit(int m);
	void set_thread_num(int n);
	void set_lock_free(int n);
	void clear_board();
	void genMove(std::string color);
	void boardState(std::vector<int>& bStones, std::vector<int>& wStones);
	bool addMove(std::string move, int color);
	void showBoard();
	bool sendCommandWithEmptyResponse(std::string command, std::string& readLine);

	static std::vector<int> bookMoves;
	static std::vector<float> estimateScore;
	
private:

	static boost::process::child c;
	bool setTerritoryParam;
	bool bookLoaded;
	boost::mutex addStone_mutex;
};
	
#endif