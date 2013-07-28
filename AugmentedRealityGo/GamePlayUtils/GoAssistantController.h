#ifndef GoAssistantController_H
#define GoAssistantController_H

////////////////////////////////////////////////////////////////////

/*
	The assistant controlelr running in a separate thread to communicate with fuego or run other assistant functions
*/
#include <boost/thread/thread.hpp>
#include <queue>
#include <iostream>
#include "../HelperClass/Helper.h"
#include "GameConstant.h"
#include "GoBoard.h"
#include "FuegoAssistant.h"
#include "JosekiAssistant.h"
#include "../HelperClass/config.h"

//test
#include <vector>
class GoAssistantController
{
public:
	GoAssistantController(GoBoard* b, Config* c);
	void pushAssistantMode(int a);
	void AssistantMainLoop();
	void playVirtualMove(std::string move, std::string color);
	void playRealMove(int move, int color);
	void showBoard();
	static std::vector<int>* FuegoBookMoves;
	static std::vector<float>* FuegoEstimateScore;
	static std::vector<cornerJoseki>* josekiMoves;

	//all those volatile variables should be changed to boost::atomic in the future
	//the memory arrangement is only guaranteed in VC
	static volatile int currentMode;
	static volatile bool isProcessing;

	static volatile int processedMove;
	static volatile bool validMove;

private:
	//use queue to store assistant request, and process in FIFO order
	std::queue<int> assistant_queue;
	std::queue<std::vector<std::string>> immediate_queue;
	static int assistant_mode;
	static FuegoAssistant fuego;
	static JosekiAssistant joseki;
	GoBoard* board;
	boost::mutex pushMutex_;
};
	
#endif