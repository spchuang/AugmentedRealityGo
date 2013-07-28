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
#define ERROR_ILLEGAL_MOVE				6


static const float CORNER_JOSEKI_COLOR[4][4]	= {{0.7f, 0.7f, 0.0f, 0.5f},
											 {0.0f, 0.7f, 0.7f, 0.5f},
											 {0.7f, 0.0f, 0.7f, 0.5f},
											 {2.0f, 0.6f, 0.5f, 0.5f}};


//assistant mode
namespace ASSISTANT_MODE{
	typedef enum
	{
	   NONE,
	   FUEGO_BOOK,
	   JOSEKI,
	   FUEGO_MOVE,
	   TERRITORY
	} MODE_NUMBER;

	const int NUMBER		=5;

}

//move submissio nmode
namespace PLAY_MODE{
	typedef enum
	{
	   NONE,
	   SUBMITTED,
	   PROCESSED
	} MODE_NUMBER;

	const int NUMBER		=5;

}

#define M_PI 3.1415926

#endif