// Video Image PSNR and SSIM
#include <iostream> // for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion



//self library
#include "ARGoController.h"



using namespace std;
//using namespace PatternLibrary;


int main(int argc, char *argv[])
{
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
	/*
	PROCESS_INFORMATION ProcessInfo; //This is what we get as an [out] parameter
	STARTUPINFO StartupInfo; //This is an [in] parameter
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof StartupInfo ; //Only compulsory field
	if(CreateProcess("C:\\WINDOWS\\System32\\notepad.exe", NULL, 
		NULL,NULL,FALSE,0,NULL,
		NULL,&StartupInfo,&ProcessInfo))
	{ 
		WaitForSingleObject(ProcessInfo.hProcess,INFINITE);
		CloseHandle(ProcessInfo.hThread);
		CloseHandle(ProcessInfo.hProcess);
	}  
	else
	{
		//MessageBox("The process could not be started...");
	} */


    ARGoController controller(640,480);
	
	controller.startAR();
	
	





	std::cout<<"back to main.."<<std::endl;
    return 0;
}