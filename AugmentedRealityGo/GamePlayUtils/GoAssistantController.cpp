#include "GoAssistantController.h"
FuegoAssistant GoAssistantController::fuego = FuegoAssistant();
JosekiAssistant GoAssistantController::joseki = JosekiAssistant();

std::vector<int>* GoAssistantController::FuegoBookMoves;
std::vector<float>* GoAssistantController::FuegoEstimateScore;
std::vector<cornerJoseki>* GoAssistantController::josekiMoves;

volatile int GoAssistantController::currentMode;
volatile bool GoAssistantController::isProcessing;

GoAssistantController::GoAssistantController(GoBoard* b, Config* c)
{
	currentMode = -1;
	board = b;
	//pass the pointer reference of fuego to board class
	b->setFuego(&fuego);
	b->setJoseki(&joseki);
	joseki.loadDB(c->joseki.dbFile, c->joseki.sgfDirectory);

	FuegoBookMoves = &(fuego.bookMoves);
	FuegoEstimateScore = &(fuego.estimateScore);
	josekiMoves		= &(joseki.josekiMoves);
	isProcessing = false;

	fuego.set_memory_limit(c->fuego.memoryLimit);
}

void GoAssistantController::pushAssistantMode(int a)
{
	if(a !=ASSISTANT_MODE::NONE){
		//not sure if this is the correct way of locking the critical area
		pushMutex_.lock();
		assistant_queue.push(a);
		pushMutex_.unlock();
	}
}
void GoAssistantController::showBoard()
{
	fuego.showBoard();
}


void GoAssistantController::AssistantMainLoop()
{
	while(1){
		if(!assistant_queue.empty()){
			//key: process the last item in queue only (in case we kept pressing and skipped a lot)
			currentMode = assistant_queue.back();
			isProcessing = true;
			//this section may be screwed up if someone push a mode during this loop..may require a lock
			pushMutex_.lock();
			while(!assistant_queue.empty()){
				assistant_queue.pop();
			}
			pushMutex_.unlock();
			switch(currentMode){
				case ASSISTANT_MODE::NONE:
					break;
				case ASSISTANT_MODE::FUEGO_BOOK:
					fprintf(stderr, "[GoAssistant]processing fuego book\n");
					fuego.getBookPositions();
					break;
				case ASSISTANT_MODE::JOSEKI:
					int nextMoveColor;
					if(board->getMoveTurnColor() == COLOR_BLACK){
						nextMoveColor = 1;
					}else{
						nextMoveColor = 2;
					}
					joseki.getJoseki(board->bStones, board->wStones, nextMoveColor);
					break;

				case ASSISTANT_MODE::FUEGO_MOVE:
					fuego.getFuegoMove(board->getMoveTurnColor());
					break;
				case ASSISTANT_MODE::TERRITORY:
					fprintf(stderr, "[GoAssistant]processing territory estimation\n");
					fuego.estimateTerritory(board->getMoveTurnColor());
					break;
			}
			fprintf(stderr, "[GoAssistant]done processing\n");
			isProcessing = false;
		}



		//check every 150 milliseconds
		boost::this_thread::sleep(boost::posix_time::milliseconds(150));
	}
}