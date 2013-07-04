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

//assistant mode
const int A_MODE_NONE		=0;
const int A_MODE_FUEGO_BOOK	=1;
#define A_MODE_TERRITORY	2;
#define NUMBER_OF_A_MODE 2;

#define M_PI 3.1415926

#endif