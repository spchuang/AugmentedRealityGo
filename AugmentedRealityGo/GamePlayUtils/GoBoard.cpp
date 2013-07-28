#include "GoBoard.h"

bool GoBoard::newMoveIsMade;
GoBoard::GoBoard()
{
	for(int i=0; i<19*19; i++){
		virtualStones[i] = COLOR_NONE;
		realStones[i] = COLOR_NONE;
		wrongRealStones[i] = NO_WRONG_MOVE;
	}
	newMoveIndex = -1;
	currentMoveColor = COLOR_BLACK;
	newMoveIsMade = false;
	bStones.clear();
	wStones.clear();
	warningMsg = NO_WRONG_MOVE;

	
}

void GoBoard::setFuego(FuegoAssistant* f)
{
	fuego = f;
}

void GoBoard::setJoseki(JosekiAssistant* j)
{
	joseki = j;
}

void GoBoard::clear_board()
{
	for(int i=0; i<19*19; i++){
		virtualStones[i] = COLOR_NONE;
		realStones[i] = COLOR_NONE;
		wrongRealStones[i] = NO_WRONG_MOVE;
	}
	warningMsg = NO_WRONG_MOVE;
	//clear fuego state too
	fuego->clear_board();
	currentMoveColor = COLOR_BLACK;
	bStones.clear();
	wStones.clear();
	newMoveIndex = -1;
}

int GoBoard::getMoveTurnColor()
{
	return currentMoveColor;
}
void GoBoard::changeTurn(int new_move)
{
	if(currentMoveColor == COLOR_WHITE)
		currentMoveColor = COLOR_BLACK;
	else if(currentMoveColor == COLOR_BLACK)
		currentMoveColor = COLOR_WHITE;

	newMoveIndex = new_move;
	newMoveIsMade = true;

	//get fuego board state
	std::vector<int> bs, ws;
	fuego->boardState(bs,ws);

	/*
	std::cerr<<"black stones: "<<std::endl;
	for(int i=0; i<bs.size();i++){
		std::cerr<<helper::convert_index_move(bs[i])<<" ";
	}
	std::cerr<<std::endl;
	std::cerr<<"white stones: "<<std::endl;
	for(int i=0; i<ws.size();i++){
		std::cerr<<helper::convert_index_move(ws[i])<<" ";
	}
	*/
	std::vector<int> removeStones;

	if(bStones.size() > bs.size()){
		//compare the list of black and white moves
		for(int i=0; i<bStones.size(); i++){
			bool exists = false;
			for(int j=0; j<bs.size(); j++){
				if(bStones[i] == bs[j]){
					exists = true;
					break;
				}
			}
			//this black stone should be removed
			if(!exists){
				removeStones.push_back(bStones[i]);
			}
		}
	}
	if(wStones.size() > ws.size()){
		for(int i=0; i<wStones.size(); i++){
			bool exists = false;
			for(int j=0; j<ws.size(); j++){
				if(wStones[i] == ws[j]){
					exists = true;
					break;
				}
			}
			//this black stone should be removed
			if(!exists){
				removeStones.push_back(wStones[i]);
			}
		}
	}
	warningMsg = NO_WRONG_MOVE;
	//remove the stones
	for(int i=0; i<removeStones.size(); i++){
		int removeIndex = removeStones[i];
		if(virtualStones[removeIndex]!= COLOR_NONE){
			virtualStones[removeIndex] = COLOR_NONE;
		}else{
			//if the real stone should be removed, put that index on the list

			wrongRealStones[removeIndex] = ERROR_REMOVE_THIS_STONE;
			realStones[removeIndex] = COLOR_NONE;
			if(warningMsg ==NO_WRONG_MOVE)
				warningMsg = ERROR_REMOVE_THIS_STONE;
		}
	}

	//update stones vectors
	wStones = ws;
	bStones = bs;
}



bool GoBoard::addVirtualStone(std::string move, std::string color)
{
	//check if this is valid move first
	if(!fuego->addMove(move, helper::convert_string_color(color)))
		
		return false;

	int stone_index = helper::convert_string_move(move);
	//std::cout<<"#add virtual stone at: "<<stone_index<<std::endl;
	if(color == "black" || color =="b"){
		virtualStones[stone_index] = COLOR_BLACK;
		bStones.push_back(stone_index);
	}else if(color =="white"|| color =="w"){
		virtualStones[stone_index] = COLOR_WHITE;
		wStones.push_back(stone_index);
	}
	
	changeTurn(stone_index);
	return true;
}

bool GoBoard::addRealStone(int stone_index, int color){
	//check if this is valid move first
	if(!fuego->addMove(helper::convert_index_move(stone_index), color)){
		wrongRealStones[stone_index] = ERROR_ILLEGAL_MOVE;
		if(warningMsg ==NO_WRONG_MOVE)
			warningMsg = ERROR_ILLEGAL_MOVE;
		return false;

	}

	if(color == COLOR_BLACK){
		bStones.push_back(stone_index);
	}else if(color == COLOR_WHITE){
		wStones.push_back(stone_index);
	}
	realStones[stone_index] = color;
	
	changeTurn(stone_index);
	return true;
}


bool GoBoard::checkNewBoardState(char newRealBoardStones[361], char newMoveColor)
{
	//before checking the new move for anything, make sure all the captuerd stones are removed from the board
	
	if(warningMsg == ERROR_REMOVE_THIS_STONE){
		bool capturedStoneRemoved = true;
		for(int i=0; i<19*19; i++)
		{

			if(wrongRealStones[i] == ERROR_REMOVE_THIS_STONE && newRealBoardStones[i]!=COLOR_NONE){
				capturedStoneRemoved = false;
			}else if(wrongRealStones[i] == ERROR_REMOVE_THIS_STONE && newRealBoardStones[i]==COLOR_NONE){
				wrongRealStones[i] = NO_WRONG_MOVE;
			}
		}
		if(!capturedStoneRemoved) return false;
	}


	//initialize a list of wrong moves
	for(int i=0; i<19*19;i++)
	{
		wrongRealStones[i] = NO_WRONG_MOVE;
	}

	warningMsg = NO_WRONG_MOVE;

	bool realOverlapVirtual = false;
	bool oldBoardStonesChanged = false;

			
	for(int i=0; i<19*19; i++)
	{
		//first check if real stone overlaps with virtual stone
		if((virtualStones[i]==COLOR_WHITE ||virtualStones[i]==COLOR_BLACK) && newRealBoardStones[i]!=COLOR_NONE)
		{
			std::cerr<<"ERROR: real stone overlaps with virtual\n";
			wrongRealStones[i] = ERROR_REAL_OVERLAPS_VIRTUAL;
			realOverlapVirtual = true;
			if(warningMsg ==NO_WRONG_MOVE)
				warningMsg = ERROR_REAL_OVERLAPS_VIRTUAL;
		}

		//check if a real stone was placed, but the new board state shows it's changed
		if( (realStones[i]==COLOR_WHITE ||realStones[i]==COLOR_BLACK) && realStones[i] != newRealBoardStones[i])
		{
			std::cerr<<"ERROR: stone state is changed\n";
			
			wrongRealStones[i] = ERROR_OLD_REAL_STONE_MOVED;
			oldBoardStonesChanged = true;
			if(warningMsg ==NO_WRONG_MOVE)
				warningMsg = ERROR_OLD_REAL_STONE_MOVED;
		}
	}

	//some stones are placed in correctly
	if(!realOverlapVirtual &&  !oldBoardStonesChanged){

		std::vector<int> allNewMoveIndex;
				
		for(int i=0; i<19*19; i++)
		{
			if( (virtualStones[i]==COLOR_NONE && realStones[i]==COLOR_NONE) && (newRealBoardStones[i]==COLOR_WHITE || newRealBoardStones[i]==COLOR_BLACK))
			{
				allNewMoveIndex.push_back(i);
			}

		}
		//check if there are more than one new moves 
		if(allNewMoveIndex.size()==0)
		{
			std::cerr<<"ERROR: didn't read the move, try again\n";

		}else if(allNewMoveIndex.size()>1){
			 std::cerr<<"ERROR: there are more than one new move\n";
			 if(warningMsg ==NO_WRONG_MOVE)
				warningMsg = ERROR_MORE_THAN_ONE_NEW_MOVES;
			for(int i=0; i<allNewMoveIndex.size(); i++)
			{
				wrongRealStones[allNewMoveIndex[i]] = ERROR_MORE_THAN_ONE_NEW_MOVES;
			}

		}else{
			//std::cout<<"#There is one move!"<<allNewMoveIndex[0]<<", "<<(int)newMoveColor<<"#"<<std::endl;
			//std::cout<<"new move color: "<<(int)newRealBoardStones[allNewMoveIndex[0]]<<std::endl;
			//only one new move, now check if it's the right color
			if(newRealBoardStones[allNewMoveIndex[0]]==newMoveColor){

				newRealMoveIndex = allNewMoveIndex[0];
				//addRealStone(allNewMoveIndex[0], newMoveColor);
				return true;
			}
			std::cerr<<"ERROR: the new stone is the wrong color!\n";
			if(warningMsg ==NO_WRONG_MOVE)
				warningMsg = ERROR_NEW_MOVE_WRONG_COLOR;
			wrongRealStones[allNewMoveIndex[0]] = ERROR_NEW_MOVE_WRONG_COLOR;
			
		}
	}
	/*
	for(int i=18; i>=0;i--){
		for(int j=0; j<19;j++)
		{
			if(wrongRealStones[i*19+j]!=NO_WRONG_MOVE)

				std::cout<<(int)wrongRealStones[i*19+j];

		}
	}
	*/
	return false;
}


std::string GoBoard::getNewMove()
{
	return  helper::convert_index_move(newMoveIndex);
}