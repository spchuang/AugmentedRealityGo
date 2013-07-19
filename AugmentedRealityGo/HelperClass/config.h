#ifndef Config_H
#define Config_H

////////////////////////////////////////////////////////////////////
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
//


#include <iostream>
#include <string>

/*
	load config file "./config.txt"
*/



class Config
{
public:
	Config(std::string config_file);
	
	struct camera{
		int height;
		int width;
		float intrinsic_array[3][3];
		float distCoeffs_array[5];
	}cam;
	
	struct boardConfiguration{
		int numPoint;
		double widthGap;
		double heightGap;
		double boardHeight;
		double boardWidth;
		double boardLength;
		double gap;
		double widthInterval;
		double heightInterval;
		double blockLength;
		int whiteStoneThresh;
		int blackStoneThresh;
	}board;

	struct markerConfiguration{
		int boardMarkerID[18];
		double markerLength;
	}marker;

	struct JosekiConfiguration{
		std::string dbFile;
		std::string sgfDirectory;
	}joseki;

	struct fuegoConfiguration{
		int memoryLimit;
	}fuego;

private:
	
	

};
	
#endif