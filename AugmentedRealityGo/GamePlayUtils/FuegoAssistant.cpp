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



FuegoAssistant::FuegoAssistant()
{



}

FuegoAssistant::~FuegoAssistant()
{
	cout<<"terminate"<<endl;
	terminate(c);


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

	string emptyRead;
	getline(readFromFuego, emptyRead);
}

bool FuegoAssistant::getBookPositions()
{
	string command = "book_position";
	writeToFuego << command<<endl;
	writeToFuego.flush();

	//read two lines,first is best move, 2nd is all posible moves

	string first_line;
	string second_line;
	getline(readFromFuego, first_line);
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
			//cout<<l[i] <<" : "<<parse_string_move(l[i])<<endl;
			bookMoves.push_back(convert_string_move(l[i]));
		}
	}

	
	//cout<<"return"<<endl<<first_line<<endl<<second_line<<endl;
    


	
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
	for(int i=0; i<22;i++){
		getline(readFromFuego, response);
		cout <<response<<endl;
	}

	cout <<"end of board"<<endl;

}