// Video Image PSNR and SSIM
#include <iostream> // for standard I/O
#include <string>   // for strings

//self library
#include "ARGoController.h"
#include "HelperClass\config.h"


using namespace std;





int main(int argc, char *argv[])
{
	
	//this code is for integrating FusekiLibrary(in C#) in the code. An easier solution is probably to just use FusekiBot
	/*
	PatternMatcher pm("Patterns.db", 19);
	pm.Load();
	__int64 positionId;
	array<SearchResult^>^ results = pm.Search("", "", GoUtils::Color::Black, 19, positionId);
	

	for(size_t i=0; i<results->Length; i++)
	{
		 //Console::WriteLine(strarray[i]->NoOfWins);
		std::cout<<results[i]->x <<" "<<results[i]->y<<std::endl;
	}

	std::cout<<"load okay"<<std::endl;
	*/

	Config config("config.json");
    ARGoController controller(&config);
	
	controller.startAR();
    return 0;
}

