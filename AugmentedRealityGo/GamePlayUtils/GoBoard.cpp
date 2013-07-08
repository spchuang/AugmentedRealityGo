#include "GoBoard.h"

bool GoBoard::newMoveIsMade;
GoBoard::GoBoard()
{
	for(int i=0; i<19*19; i++){
		virtualStones[i] = 2;
		realStones[i] = 2;
		wrongRealStones[i] = 2;
	}
	currentMoveColor = COLOR_BLACK;
	newMoveIsMade = false;
}

void GoBoard::setFuego(FuegoAssistant* f)
{
	fuego = f;
}



void GoBoard::clear_board()
{
	
	for(int i=0; i<19*19; i++){
		virtualStones[i] = 2;
		realStones[i] = 2;
		wrongRealStones[i] = 2;
	}
	//clear fuego state too
	fuego->clear_board();
	currentMoveColor = COLOR_WHITE;
}

int GoBoard::getMoveTurnColor()
{
	return currentMoveColor;
}
void GoBoard::changeTurn()
{
	if(currentMoveColor == COLOR_WHITE)
		currentMoveColor = COLOR_BLACK;
	else if(currentMoveColor == COLOR_BLACK)
		currentMoveColor = COLOR_WHITE;
	newMoveIsMade = true;
}

bool GoBoard::addVirtualStone(std::string move, std::string color)
{
	//check if this is valid move first
	if(!fuego->addMove(move, helper::convert_string_color(color)))
		return false;
	int stone_index = helper::convert_string_move(move);
	//std::cout<<"#add virtual stone at: "<<stone_index<<std::endl;
	if(color == "black" || color =="b")
		virtualStones[stone_index] = COLOR_BLACK;
	else if(color =="white"|| color =="w")
		virtualStones[stone_index] = COLOR_WHITE;
	
	changeTurn();
	return true;
}

bool GoBoard::addRealStone(int stone_index, int color){
	//check if this is valid move first
	if(!fuego->addMove(helper::convert_index_move(stone_index), color))
		return false;
	realStones[stone_index] = color;
	changeTurn();
	return true;
}


bool GoBoard::checkNewBoardState(char newRealBoardStones[361], char newMoveColor)
{
	//initialize a list of wrong moves
	for(int i=0; i<19*19;i++)
	{
		wrongRealStones[i] = NO_WRONG_MOVE;

	}
	bool realOverlapVirtual = false;
	bool oldBoardStonesChanged = false;

			
	for(int i=0; i<19*19; i++)
	{
		//first check if real stone overlaps with virtual stone
		if((virtualStones[i]==0 ||virtualStones[i]==1) && newRealBoardStones[i]!=2)
		{
			fprintf(stderr, "ERROR: real stone overlaps with virtual\n");
			wrongRealStones[i] = ERROR_REAL_OVERLAPS_VIRTUAL;
			realOverlapVirtual = true;
		}

		//check if a real stone was placed, but the new board state shows it's changed
		if( (realStones[i]==0 ||realStones[i]==1) && realStones[i] != newRealBoardStones[i])
		{
			fprintf(stderr, "ERROR: stone state is changed\n");

			wrongRealStones[i] = ERROR_OLD_REAL_STONE_MOVED;
			oldBoardStonesChanged = true;
	
		}
	}

	//some stones are placed in correctly
	if(!realOverlapVirtual &&  !oldBoardStonesChanged){

		std::vector<int> allNewMoveIndex;
				
		for(int i=0; i<19*19; i++)
		{
			if( (virtualStones[i]==2 && realStones[i]==2) && (newRealBoardStones[i]==0 || newRealBoardStones[i]==1))
			{
				allNewMoveIndex.push_back(i);
			}

		}
		//check if there are more than one new moves 
		if(allNewMoveIndex.size()==0)
		{
			fprintf(stderr, "ERROR: didn't read the move, try again\n");

		}else if(allNewMoveIndex.size()>1){
			fprintf(stderr, "ERROR: there are more than one new move\n");

			for(int i=0; i<allNewMoveIndex.size(); i++)
			{
				wrongRealStones[allNewMoveIndex[i]] = ERROR_MORE_THAN_ONE_NEW_MOVES;
			}

		}else{
			//std::cout<<"#There is one move!"<<allNewMoveIndex[0]<<", "<<(int)newMoveColor<<"#"<<std::endl;
			//std::cout<<"new move color: "<<(int)newRealBoardStones[allNewMoveIndex[0]]<<std::endl;
			//only one new move, now check if it's the right color
			if(newRealBoardStones[allNewMoveIndex[0]]==newMoveColor){

				newMoveIndex = allNewMoveIndex[0];
				//addRealStone(allNewMoveIndex[0], newMoveColor);
				return true;
			}
			fprintf(stderr, "ERROR: the new stone is the wrong color!\n");

			wrongRealStones[allNewMoveIndex[0]] = ERROR_NEW_MOVE_WRONG_COLOR;
			
		}
	}
	return false;
}


std::string GoBoard::getNewMove()
{
	return  helper::convert_index_move(newMoveIndex);
}