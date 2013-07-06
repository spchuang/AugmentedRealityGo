#include "FuegoAssistant.h"

using namespace std;
using namespace boost::process;
using namespace boost::process::initializers;
using namespace boost::iostreams;


//spawn child process to run fuego
boost::process::pipe write_to_child_pipe = create_pipe();
boost::process::pipe read_from_child_pipe = create_pipe();

file_descriptor_sink child_sink(read_from_child_pipe.sink, close_handle);
file_descriptor_source child_source(write_to_child_pipe.source, close_handle);

boost::process::child FuegoAssistant::c = 
	execute(
        run_exe("Fuego/fuego.exe"),
        bind_stdout(child_sink),
		bind_stdin(child_source)
    );

file_descriptor_source parent_source(read_from_child_pipe.source, close_handle); //read data from child
file_descriptor_sink parent_sink(write_to_child_pipe.sink, close_handle); //write data to child

stream<file_descriptor_source> readFromFuego =  stream<file_descriptor_source>(parent_source);
stream<file_descriptor_sink> writeToFuego = stream<file_descriptor_sink>(parent_sink);
//end binding pipes 
vector<int> FuegoAssistant::bookMoves;
vector<float> FuegoAssistant::estimateScore;


FuegoAssistant::FuegoAssistant()
{
	//set the territory statistic flag in fuego

	setTerritoryParam = false;
	prevMove = "none";
	
	for(int i=0; i<19*19; i++)
	{
		estimateScore.push_back(0);
	}

}

FuegoAssistant::~FuegoAssistant()
{
	cout<<"terminate"<<endl;
	terminate(c);


}
void FuegoAssistant::clear_board()
{
	sendCommandWithEmptyResponse("clear_board");

}
void FuegoAssistant::sendCommandWithEmptyResponse(string command)
{
	writeToFuego << command<<endl;
	writeToFuego.flush();

	string readLine;
	do{
		getline(readFromFuego, readLine);

	}while(readLine[0]!='=');
	cout <<command<<endl<<readLine<<endl;
}

bool FuegoAssistant::estimateTerritory(int color)
{
	string command;
	string readLine;
	//uct_param_globalsearch territory_statistics 1
	if(!setTerritoryParam){
		sendCommandWithEmptyResponse("uct_param_globalsearch territory_statistics 1");
		setTerritoryParam = true;
	}

	//the problem here is that i need to generate a move from fuego first before i can see the territory stats
	//first, disable tree search (node threhold normally = 3)
	//uct_param_search expand_threshold 10000000   
	sendCommandWithEmptyResponse("uct_param_search expand_threshold 10000000");
	
	//if there are book moves, should probably disable book first
	command = "book_moves";
	writeToFuego << command<<endl;
	writeToFuego.flush();
	do{
		getline(readFromFuego, readLine);
	}while(readLine[0]!='=');	
	vector<string> l;
	if(split(readLine, l, ' ') >1)
	{
		//needs to remove book
		sendCommandWithEmptyResponse("book_clear");
	}

	//generate a fake move from fuego
	sendCommandWithEmptyResponse(string("genmove ")+ convert_int_color(color));
	//undo the move
	sendCommandWithEmptyResponse("undo");
	//get territory stats
	command = "uct_stat_territory";
	writeToFuego << command<<endl;
	writeToFuego.flush();
	do{
		getline(readFromFuego, readLine);

	}while(readLine[0]!='=' && readLine[0]!='?');
	
	if(readLine[0] == '?'){
		cout <<"something is wrong probably cuz fuego uses forced opening moves"<<endl;
		for(int i=0; i<19*19; i++){
			estimateScore[i] = 0;
		}
		return false;
	}

	
	//the line after?

		getline(readFromFuego, readLine);

		cout<<"return: "<<endl<<readLine<<endl<<"END"<<endl;
		cout <<"is this okay?"<<endl;
		split(readLine, l, ' ');

		cout <<"size: "<< l.size()<<endl;
		for(int z=0; z<l.size(); z++)
		{
			cout <<z << " : " << l[z] <<endl;

		}
		

	
	


	//cout<<endl<<endl<<readLine<<endl<<endl;
	
	
	


	
	return true;

}

void FuegoAssistant::addMove(std::string move, int color){
	//realStones[stone_index] = color;
	string command;
	if(color == COLOR_BLACK)
		command = "play b "+move;
	else if(color == COLOR_WHITE)
		command = "play w "+move;
	writeToFuego << command<<endl;
	writeToFuego.flush();

	string readLine;
	do{
		getline(readFromFuego, readLine);
	}while(readLine[0]!='=');
	

	//getBookPositions();
}

bool FuegoAssistant::getBookPositions()
{
	string command = "book_position";
	writeToFuego << command<<endl;
	writeToFuego.flush();

	//read two lines,first is best move, 2nd is all posible moves

	string first_line;
	string second_line;
	//make sure the first line starts with =
	do{
		getline(readFromFuego, first_line);
	}while(first_line[0]!='=');
	
	getline(readFromFuego, second_line);

	bookMoves.clear();
	
	vector<string> l;
	//parse first line
	transform(first_line.begin(), first_line.end(), first_line.begin(), ::tolower);
	if(split(first_line, l, ' ') <4)
		return false;
	
	bookMoves.push_back(convert_string_move(l[3]));

	//parse second line
	transform(second_line.begin(), second_line.end(), second_line.begin(), ::tolower);
	if(split(second_line, l, ' ') >=3){
		for(size_t i=2; i<l.size(); i++){
			//cout<<l[i] <<" : "<<convert_string_move(l[i])<<endl;
			bookMoves.push_back(convert_string_move(l[i]));
		}
	}


	
	return true;
}
void FuegoAssistant::genMove(string color)
{
	string command;
	if(color == "black" || color =="b")
		command = "genmove b";
	else if(color =="white"|| color =="w")
		command = "genmove w";
	writeToFuego << command<<endl;
	writeToFuego.flush();

	std::string s;
    std::getline(readFromFuego, s);

	cout<<"return"<<endl<<s<<endl;
}

void FuegoAssistant::showBoard()
{
	string command = "showboard";
	writeToFuego << command<<endl;
	writeToFuego.flush();

	string response;
	//make sure first line starts with =
	do{
		getline(readFromFuego, response);
	}while(response[0]!='=');

	for(int i=0; i<21;i++){
		getline(readFromFuego, response);
		cout <<response<<endl;
	}



}