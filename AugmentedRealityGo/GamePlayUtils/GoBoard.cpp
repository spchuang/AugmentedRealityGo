#include "GoBoard.h"

FuegoAssistant* GoBoard::fuego;
GoBoard::GoBoard(FuegoAssistant* f)
{
	a=0;
	for(int i=0; i<19*19; i++){
		virtualStones[i] = 2;
		realStones[i] = 2;
		wrongRealStones[i] = 2;
	}
	fuego = f;
}




void GoBoard::addVirtualStone(std::string move, std::string color)
{
	int stone_index = convert_string_move(move);
	//std::cout<<"#add virtual stone at: "<<stone_index<<std::endl;
	if(color == "black" || color =="b")
		virtualStones[stone_index] = COLOR_BLACK;
	else if(color =="white"|| color =="w")
		virtualStones[stone_index] = COLOR_WHITE;
	fuego->addMove(move, convert_string_color(color));
}

void GoBoard::addRealStone(int stone_index, int color){
	realStones[stone_index] = color;
	fuego->addMove(convert_index_move(stone_index), color);
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
			std::cout<<"#real stone overlaps with virtual#"<<std::endl;
			wrongRealStones[i] = ERROR_REAL_OVERLAPS_VIRTUAL;
			realOverlapVirtual = true;
		}

		//check if a real stone was placed, but the new board state shows it's changed
		if( (realStones[i]==0 ||realStones[i]==1) && realStones[i] != newRealBoardStones[i])
		{
			std::cout<<"#stone state is changed#"<<std::endl;
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
			std::cout<<"#didn't read the move, try again#"<<std::endl;
		}else if(allNewMoveIndex.size()>1){
			std::cout<<"#move than one new move#"<<std::endl;
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
				addRealStone(allNewMoveIndex[0], newMoveColor);
				return true;
			}
			std::cout<<"#the new stone is the wrong color!#"<<std::endl;
			wrongRealStones[allNewMoveIndex[0]] = ERROR_NEW_MOVE_WRONG_COLOR;
			
		}
	}
	return false;
}


std::string GoBoard::getNewMove()
{
	return  convert_index_move(newMoveIndex);
}