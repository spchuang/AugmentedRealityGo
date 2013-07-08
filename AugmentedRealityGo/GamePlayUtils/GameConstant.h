#ifndef GameConstant_H
#define GameConstant_H

#define COLOR_BLACK 0
#define COLOR_WHITE 1
#define COLOR_NONE  2

//0: no wrong move
//1: real stone overlaps with virtual stone
//2: old real stone was moved
//3: more than one new moves
//4: new move is wrong color

#define NO_WRONG_MOVE					0
#define ERROR_REAL_OVERLAPS_VIRTUAL		1
#define ERROR_OLD_REAL_STONE_MOVED		2
#define ERROR_MORE_THAN_ONE_NEW_MOVES	3
#define ERROR_NEW_MOVE_WRONG_COLOR		4
#define ERROR_REMOVE_THIS_STONE			5

//assistant mode
namespace ASSISTANT_MODE{
	typedef enum
	{
	   NONE,
	   FUEGO_BOOK,
	   TERRITORY
	} MODE_NUMBER;
	//const int NONE			=0;
	//const int FUEGO_BOOK	=1;
	//const int TERRITORY		=2;
	const int NUMBER		=3;

}



#define M_PI 3.1415926

#endif