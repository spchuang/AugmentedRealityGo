#ifndef HELPER_H
#define HELPER_H


#include <string>
#include <sstream>
#include <iostream>
#include "GameConstant.h"

namespace helper
{

	//move has to be in lower case
	//convert string move (ex. A1) to index for board (ex. 0)
	static int convert_string_move(const std::string move)
	{
		
		int x = (int)(move[0]-'a');
		if(x>8)
			x--;
		std::string num = move.substr(1);
		int y = (int)(atoi(num.c_str()));
		//cout << x <<" , "<<y<<endl;
		return (19*(y-1)+x);
	}
	//convert index(int) move to string format
	static std::string convert_index_move(const int move)
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

	//convert color in int to string format
	static std::string convert_int_color(const int color)
	{
		if(color == COLOR_BLACK)
			return std::string("b");
		else if(color == COLOR_WHITE)
			return std::string("w");


	}

	//convert string format color to int
	static int convert_string_color(const std::string color)
	{
		if(color == "black" || color =="b")
			return COLOR_BLACK;
		else if(color =="white"|| color =="w")
			return COLOR_WHITE;
		return -1;
	}

	
	//split a string to a vector of substrings based on the separator (ch)
	static unsigned int split(const std::string &txt, std::vector<std::string> &strs, char ch)
	{
		unsigned int pos = txt.find( ch );
		unsigned int initialPos = 0;
		strs.clear();

		// Decompose statement
		while( pos != std::string::npos ) {
			/*
		
			if(txt.substr( initialPos, pos - initialPos + 1 )[0] != ch){
				std::cout<<"substr: "<< txt.substr( initialPos, pos - initialPos + 1 ) <<std::endl;
				 strs.push_back( txt.substr( initialPos, pos - initialPos + 1 ) );
			}
		   */
		
			strs.push_back( txt.substr( initialPos, pos - initialPos + 1 ) );
			initialPos = pos + 1;

			pos = txt.find( ch, initialPos );
		
			/*
			while(txt[pos+1]== ch)
				pos++;
			*/
		}

		// Add the last one
		strs.push_back( txt.substr( initialPos,(std::min)( pos, txt.size() ) - initialPos + 1 ) );

		return strs.size();
	}

	static std::string trim(std::string str)
	{
		//trim input
		char const* whiteSpace = " \t\r";
		size_t pos = str.find_first_not_of(whiteSpace);
		str.erase(0, pos);
		pos = str.find_last_not_of(whiteSpace);
		str.erase(pos + 1); 
		return str;
	}

	template<class T>
	static std::string add_var_to_string(std::string const &a, const T &b){
	  std::ostringstream oss;
	  oss<<a<<b;
	  return oss.str();
	}
}

#endif