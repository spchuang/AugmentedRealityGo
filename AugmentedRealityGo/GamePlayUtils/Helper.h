#ifndef HELPER_H
#define HELPER_H


#include <string>
#include "GameConstant.h"
//move has to be in lower case
static int convert_string_move(std::string move)
{
	int x = (int)(move[0]-'a');
	if(x>8)
		x--;
	std::string num = move.substr(1);
	int y = (int)(atoi(num.c_str()));
	//cout << x <<" , "<<y<<endl;
	return (19*(y-1)+x);
}

static std::string convert_int_color(int color)
{
	if(color == COLOR_BLACK)
		return std::string("b");
	else if(color == COLOR_WHITE)
		return std::string("w");


}

static int convert_string_color(std::string color)
{
	if(color == "black" || color =="b")
		return COLOR_BLACK;
	else if(color =="white"|| color =="w")
		return COLOR_WHITE;
	return -1;
}

static std::string convert_index_move(int move)
{
	
	char x = move%19 + 'A';
	if(move%19 > 8)
		x++;
	int y = (int)(move/19)+1;
	//create a stringstream
	std::stringstream ss;
	//add number to the stream
	ss << y;
	return x+ss.str();
}

static unsigned int split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
    unsigned int pos = txt.find( ch );
    unsigned int initialPos = 0;
    strs.clear();

    // Decompose statement
    while( pos != std::string::npos ) {
		std::cout<<"substr: "<< txt.substr( initialPos, pos - initialPos + 1 ) <<std::endl;
        strs.push_back( txt.substr( initialPos, pos - initialPos + 1 ) );
        initialPos = pos + 1;

        pos = txt.find( ch, initialPos );
		
		while(txt[pos+1]== ch)
			pos++;
		
    }

    // Add the last one
    strs.push_back( txt.substr( initialPos,(std::min)( pos, txt.size() ) - initialPos + 1 ) );

    return strs.size();
}

#endif