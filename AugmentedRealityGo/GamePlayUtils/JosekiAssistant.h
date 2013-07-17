#ifndef JosekiAssistant_H
#define JosekiAssistant_H

////////////////////////////////////////////////////////////////////
#include <string>
#include <fstream>
#include <iostream> // for standard I/O
#include <boost/filesystem.hpp> 

#include "../HelperClass/Helper.h"
#include "kombilo/sgfparser.h"
#include "kombilo/search.h"
#include "GameConstant.h"
/*
	wrapper to communicate with fuego 
*/
using boost::filesystem::directory_iterator;
using namespace std;

//i know circular pointers are bad but it's an easier design

class JosekiAssistant
{
public:
	JosekiAssistant();

	void loadDB(string dbFile, string sgfFolder);
	void getJoseki(std::vector<int> temp_bStones,std::vector<int> temp_wStones);
	~JosekiAssistant();
	
	
private:
	string createCornerPatternString(int sw, int width, int height, std::vector<int>& temp_bStones,std::vector<int>& temp_wStones);
	GameList* gl;
	int algos;


};
	
#endif