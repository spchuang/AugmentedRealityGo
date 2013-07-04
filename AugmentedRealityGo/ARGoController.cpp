#include "ARGoController.h"

#include <iostream> // for standard I/O
#include <string>

using namespace std;
FuegoAssistant ARGoController::fuego = FuegoAssistant();
GoBoard ARGoController::board = GoBoard(&fuego);



ARGoController::ARGoController(int sw, int sh)
	:graphic_controller(sw, sh, &board, &fuego)
{
	
	
}
ARGoController::~ARGoController()
{
	
	

}
vector<string> parse_command(string command)
{
	//trim input
	char const* whiteSpace = " \t\r";
	size_t pos = command.find_first_not_of(whiteSpace);
	command.erase(0, pos);
	pos = command.find_last_not_of(whiteSpace);
	command.erase(pos + 1); 

	size_t p =0;
	vector<string> arguments;
	while(command.size()>0)
	{
		p = command.find_first_of(whiteSpace);
		arguments.push_back(command.substr(0, p));
		command.erase(0, command.find_first_not_of(whiteSpace, p));
	}

	return arguments;

}



void ARGoController::startAR()
{
	//graphic_controller.test = 0;
	char *argv [1];
	int argc=1;
	argv [0]=_strdup ("ARGo");
	graphic_handler = new boost::thread(boost::bind(&ARGraphicController::start, &graphic_controller, argc, argv));
	//boost::thread workerThread(workerFunc); 
	//graphic_controller.start(argc,argv)
	//graphic_controller.global_data = 0;
	//read from cin
	string command_line;

	/*
	while(1)
	{
		cout<<"in main control: a: "<<board.a<<endl;
		
		graphic_handler->timed_join(boost::posix_time::seconds(1));
	}*/
	while(1)
	{
		getline(cin, command_line);
		//convert to lowercase
		transform(command_line.begin(), command_line.end(), command_line.begin(), ::tolower);
		//simple way to test output
		vector<string> arguments = parse_command(command_line);
		string command = arguments[0];
		
		//for(int i=0; i<arguments.size(); i++)
		//	cout <<arguments[i] << endl;

		string response = "= ";
		
		if(command == "protocol_version"){
			response += "2";
		}else if(command=="test"){
			
		
		}else if(command == "name"){
			response += "ARGo";
		}else if(command == "version"){
			response += "1.0";
		}else if(command == "known_commands"){

		}else if(command == "list_commands"){
			response+= "protocol_version\nname\nversion\nknown_command\nlist_commands\nquit\nboardsize\nclear_board\nkomi\nplay\ngenmove\ngogui-analyze_commands\nfb-info";

		}else if(command == "quit"){

		}else if(command == "boardsize"){

		}else if(command == "clear_board"){

		}else if(command == "komi"){

		}else if(command == "play"){
			
			board.addVirtualStone(arguments[2], arguments[1]);

		}else if(command == "genmove"){
			char color;
			if(arguments[1] == "b" || arguments[1] == "black"){
				color = 0;
			}else if(arguments[1] == "w" || arguments[1] == "white"){
				color = 1;
			}
			//wait for moves 
			graphic_controller.changeGen(color);
			while(1)
			{

				if(!graphic_controller.genMove)
				{
					//cout << (int)graphic_controller.newMove[0] << " , " <<(int)graphic_controller.newMove[1] <<endl;
					
					response+= board.getNewMove();
					//graphic_controller.newMove[0] = -1;
					//graphic_controller.newMove[1] = -1;
					break;
				}
				

			}

		}else if(command == "gogui-analyze_commands"){
			response += "gfx/Show Information/fb-info";

		}
	
		cout <<response <<endl<<endl;
	}
	delete graphic_handler;
	std::cout<<"done2.."<<std::endl;
}