#ifndef GoAssistantController_H
#define GoAssistantController_H

////////////////////////////////////////////////////////////////////

/*
	The assistant controlelr running in a separate thread to communicate with fuego or run other assistant functions
*/
#include <boost/thread/thread.hpp>
#include <queue>
#include <iostream>
#include "GameConstant.h"
#include "Helper.h"
#include "FuegoAssistant.h"
#include "GoBoard.h"

class GoAssistantController
{
public:
	GoAssistantController(GoBoard* b);
	void pushAssistantMode(int a);
	void AssistantMainLoop();

	static std::vector<int>* FuegoBookMoves;
	static std::vector<float>* FuegoEstimateScore;

	//all those volatile variables should be changed to boost::atomic in the future
	//the memory arrangement is only guaranteed in VC
	static volatile int currentMode;
	static volatile bool isProcessing;
private:
	//use queue to store assistant request, and process in FIFO order
	std::queue<int> assistant_queue;
	static int assistant_mode;
	static FuegoAssistant fuego;
	GoBoard* board;
	boost::mutex pushMutex_;
};
	
#endif