#include "ARGoController.h"

#include <iostream> // for standard I/O
#include <string>

using namespace std;
//FuegoAssistant ARGoController::fuego = FuegoAssistant();
GoBoard ARGoController::board = GoBoard();



ARGoController::ARGoController(int sw, int sh)
	:assistant_controller(&board)
	,graphic_controller(sw, sh, &board, &assistant_controller)
{
	
	
}
ARGoController::~ARGoController()
{
	delete graphic_handler;
	delete assistant_handler;
	fprintf(stderr, "ARGoController destructed\n");

	
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
	//spawn thread for graphic controller
	char *argv [1];
	int argc=1;
	argv [0]=_strdup ("ARGo");
	graphic_handler = new boost::thread(boost::bind(&ARGraphicController::start, &graphic_controller, argc, argv));

	//spawn thread for assistant controller
	assistant_handler = new boost::thread(boost::bind(&GoAssistantController::AssistantMainLoop, &assistant_controller));

	//boost::thread workerThread(workerFunc); 
	//graphic_controller.start(argc,argv)
	//graphic_controller.global_data = 0;
	//read from cin
	string command_line;

	while(1)
	{
		getline(cin, command_line);
		//convert to lowercase
		transform(command_line.begin(), command_line.end(), command_line.begin(), ::tolower);
		//simple way to test output
		vector<string> arguments = parse_command(command_line);
		string command = arguments[0];
		

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
			board.clear_board();
		}else if(command == "komi"){

		}else if(command == "play"){
			if(arguments.size() == 3)
				board.addVirtualStone(arguments[2], arguments[1]);
			else
				response = "? invalid play move";

		}else if(command == "genmove"){
			char color;
			if(arguments[1] == "b" || arguments[1] == "black"){
				color = COLOR_BLACK;
			}else if(arguments[1] == "w" || arguments[1] == "white"){
				color = COLOR_WHITE;
			}
			//wait for moves 
			graphic_controller.changeGen(color);
			while(1)
			{

				if(!graphic_controller.genMove)
				{
					//cout << (int)graphic_controller.newMove[0] << " , " <<(int)graphic_controller.newMove[1] <<endl;
					
					response+= board.getNewMove();
					break;
				}
				

			}

		}else if(command == "gogui-analyze_commands"){
			response += "gfx/Show Information/fb-info";

		}else if(command=="fb-info"){

		}
	
		cout <<response <<endl<<endl;
	}
	delete graphic_handler;
	std::cout<<"done2.."<<std::endl;
}