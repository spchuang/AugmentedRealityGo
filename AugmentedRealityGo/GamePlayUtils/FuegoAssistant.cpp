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
	
	for(int i=0; i<19*19; i++)
	{
		estimateScore.push_back(0);
	}
	bookLoaded = true;

}

FuegoAssistant::~FuegoAssistant()
{
	fprintf(stderr, "[FuegoAssistant]terminate\n");
	terminate(c);


}
void FuegoAssistant::clear_board()
{
	string readLine;
	sendCommandWithEmptyResponse("clear_board", readLine);

}
bool FuegoAssistant::sendCommandWithEmptyResponse(string command, string& readLine)
{
	writeToFuego << command<<endl;
	writeToFuego.flush();

	do{
		getline(readFromFuego, readLine);

	}while(readLine[0]!='=' && readLine[0]!='?');

	if(readLine[0] == '?'){
		return false;
	}

	fprintf(stderr, "[FuegoAssistant]%s\n[FuegoAssistant]%s\n\n", command.c_str(), readLine.c_str());
	return true;
}

bool FuegoAssistant::estimateTerritory(int color)
{
	string command;
	string readLine;
	//uct_param_globalsearch territory_statistics 1
	if(!setTerritoryParam){
		sendCommandWithEmptyResponse("uct_param_globalsearch territory_statistics 1", readLine);
		setTerritoryParam = true;
	}

	//the problem here is that i need to generate a move from fuego first before i can see the territory stats
	//first, disable tree search (node threhold normally = 3)
	//uct_param_search expand_threshold 10000000   
	sendCommandWithEmptyResponse("uct_param_search expand_threshold 10000000", readLine);
	
	//if there are book moves, should probably disable book first
	command = "book_moves";
	writeToFuego << command<<endl;
	writeToFuego.flush();
	do{
		getline(readFromFuego, readLine);
	}while(readLine[0]!='=');	
	vector<string> l;
	if(helper::split(readLine, l, ' ') >1)
	{
		//needs to remove book
		sendCommandWithEmptyResponse("book_clear", readLine);
		bookLoaded =false;
	}

	//generate a fake move from fuego
	sendCommandWithEmptyResponse(string("genmove ")+ helper::convert_int_color(color), readLine);
	//undo the move
	sendCommandWithEmptyResponse("undo", readLine);

	//get territory stats
	if(!sendCommandWithEmptyResponse("uct_stat_territory", readLine)){
		fprintf(stderr, "[FuegoAssistant]something is wrong probably cuz fuego uses forced opening moves\n");
		for(int i=0; i<19*19; i++){
			estimateScore[i] = 0;
		}
		return false;
	}

	
	//save the result
	for(int i=18; i>=0; i--){
		getline(readFromFuego, readLine);

		//cout<<"return: "<<endl<<readLine<<endl<<"END"<<endl;
		//cout <<"is this okay?"<<endl;
		helper::split(readLine, l, ' ');

		int j=0;
		for(int z=0; z<l.size(); z++)
		{
			if(l[z][0] != ' '){
				estimateScore[i*19+j] = ::atof(l[z].c_str());
				j++;
			}

		}
	}	

	//load the book back
	if(!bookLoaded){
		command = "book_load  \"Fuego\\book.dat\"";
		if(!sendCommandWithEmptyResponse(command, readLine)){
			fprintf(stderr, "[FuegoAssistant]%s\n\n", readLine);

		}
	}
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
	if(helper::split(first_line, l, ' ') <4)
		return false;
	
	bookMoves.push_back(helper::convert_string_move(l[3]));

	//parse second line
	transform(second_line.begin(), second_line.end(), second_line.begin(), ::tolower);
	if(helper::split(second_line, l, ' ') >=3){
		for(size_t i=2; i<l.size(); i++){
			bookMoves.push_back(helper::convert_string_move(l[i]));
		}
	}


	
	return true;
}

//not sure why i need this..
void FuegoAssistant::genMove(string color)
{
	string command;
	string readLine;
	if(color == "black" || color =="b")
		command = "genmove b";
	else if(color =="white"|| color =="w")
		command = "genmove w";

	sendCommandWithEmptyResponse(command, readLine);


	cout<<"return"<<endl<<readLine<<endl;
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

	fprintf(stderr, "[FuegoAssistant]Board State:\n");
	for(int i=0; i<21;i++){
		getline(readFromFuego, response);
		fprintf(stderr, "%s\n", response);

	}



}