#include "JosekiAssistant.h"

std::vector<cornerJoseki> JosekiAssistant::josekiMoves;

JosekiAssistant::JosekiAssistant()
{
	algos = ALGO_FINALPOS | ALGO_MOVELIST | ALGO_HASH_FULL | ALGO_HASH_CORNER;
}

JosekiAssistant::~JosekiAssistant()
{
	delete gl;
}



void JosekiAssistant::loadDB(string dbFile, string sgfFolder)
{
	// ----------------- set up processing options -----------------------------------
	ProcessOptions* p_op = new ProcessOptions;

	// ----------------- check if db exists -----------------------------------------
	bool DBexists = false;

	if(boost::filesystem::exists("joseki/joseki.db")){
		DBexists = true;
		cerr <<"[JosekiAssistant]Joseki database exists"<<endl;
	}else{
		DBexists = false;
		cerr<< "[JosekiAssistant]No joseki database file: start processing" <<endl;
	}

	// ----------------- create GameList instance -----------------------------------
	gl = new GameList(dbFile.c_str(),
      "id", "[[PW]] - [[PB]] ([[winner]]), [[filename.]], ", 0);


	//if no database file exists yet
	if(!DBexists){
		// ---------------- process SGF games ---------------------------------------------
		vector<GameList* > glv;
		// glv.push_back(&gl1);
		gl->start_processing();
		directory_iterator end_itr;

		int counter = 0;
		for(directory_iterator it(sgfFolder); it != end_itr; ++it) {
		  string n = it->path().filename().string();
		  if (n.substr(n.size()-4) == ".sgf") {
			ifstream infile;
			infile.open(it->path().string().c_str());

			string sgf;
			string line;
			while (!infile.eof()) {
			  getline(infile, line);
			  sgf += line + "\n";
			}
			infile.close();
			int flags = CHECK_FOR_DUPLICATES| CHECK_FOR_DUPLICATES_STRICT; // | OMIT_DUPLICATES 
			if (gl->process(sgf.c_str(), sgfFolder.c_str(), n.c_str(), glv, "", flags)) {
				if (gl->process_results()) 
					cerr <<"process res "<< gl->process_results() <<endl;
				if (gl->process_results() & IS_DUPLICATE) 
					cerr <<"is duplicate: " << counter <<endl;
				  
			}
			counter++;
		  }
		}

		gl->finalize_processing();
	}

	cerr<<"[JosekiAssistant]Total of "<<gl->size() <<" games." <<endl;
	delete p_op;
}


void JosekiAssistant::searchCornerJoseki(int corner, int sw, int width, int height, 
	std::vector<int>& temp_bStones,std::vector<int>& temp_wStones)
{
	//during this process kinda assume no stones are being played...
	string patternString;
	for(int i=height-1; i>=0; i--){

		for(int j=0; j<width; j++){
			int id = sw+i*19+j;
			bool stonePlaced = false;

			//put black stone to pattern
			for(size_t bs=0; bs< temp_bStones.size(); bs++){
				if(temp_bStones[bs] == id){

					patternString+="X";
					stonePlaced = true;
					break;
				}
			}

			//put white stone to pattern
			for(size_t ws=0; ws< temp_wStones.size(); ws++){
				if(temp_wStones[ws] == id){
					patternString+="O";
					stonePlaced = true;
					break;
				}
			}
			if(!stonePlaced)
				patternString+=".";
		}
	}
	// ------------------- set up search pattern --------------------------------
	Pattern p(corner,19,width,height, patternString.c_str());
	//printf("Search pattern:\n");
	//printf("%s\n", p.printPattern().c_str());

	// -------------------- do pattern search --------------------------------------
	gl->reset();
	gl->search(p, &so);

	//printf("Continuations:\n");
	//print joseki result
	for(int y=0; y<p.sizeY; y++) {
		for(int x=0; x<p.sizeX; x++) {
			
			//printf("%c", gl->lookupLabel(x,y));

			if(gl->lookupLabel(x,y) != '.'){
				int id = sw+(height-y-1)*19+(x);
				cornerJoseki j;
				j.id = id;
				j.corner = corner;
				//printf("%c: (%d, %d) = %d\n",gl->lookupLabel(x,y), x, sizeY-y-1, id);
				josekiMoves.push_back(j);
			}
		}
		//cout<<endl;
	}

}

void JosekiAssistant::getJoseki(std::vector<int> temp_bStones,std::vector<int> temp_wStones, int nextMove)
{
	int sizeX = 10;
	int sizeY = 10;

	josekiMoves.clear();
	//save a temporarly board state
	//temp_bStones = board->bStones;
	//temp_wStones = board->wStones;

	// -------------------- set up search options ----------------------------------
	so.fixedColor = 0;
	so.algos = algos;
	so.nextMove = nextMove;

	// ------------------- set up search pattern for 4 corners of the board---------
	searchCornerJoseki(CORNER_SW_PATTERN, 0, sizeX, sizeY, temp_bStones, temp_wStones);
	searchCornerJoseki(CORNER_SE_PATTERN, 9, sizeX, sizeY, temp_bStones, temp_wStones);
	searchCornerJoseki(CORNER_NW_PATTERN, 171, sizeX, sizeY, temp_bStones, temp_wStones);
	searchCornerJoseki(CORNER_NE_PATTERN, 180, sizeX, sizeY, temp_bStones, temp_wStones);


	
	/*
	printf("\n");
		printf("Statistics:\n"); 
		printf("num hits: %d, num switched: %d, B wins: %d, W wins: %d\n", gl.num_hits, gl.num_switched, gl.Bwins, gl.Wwins);


		printf("Continuation | Black      ( B wins / W wins ) | White      (B wins / W wins) |\n");
		for(int y=0; y<p.sizeY; y++) {
			for(int x=0; x<p.sizeX; x++) {
				if (gl->lookupLabel(x,y) != '.') {
				Continuation cont = gl->lookupContinuation(x,y);
				printf("      %c      |   %3d[%3d] (    %3d /    %3d ) |   %3d[%3d] (   %3d /    %3d) | %1.1f /  %1.1f | %d | %d |  \n",
					gl->lookupLabel(x,y), cont.B, cont.tB, cont.wB, cont.lB, cont.W, cont.tW, cont.wW, cont.lW, 
					cont.wW*100.0/cont.W, cont.wB*100.0/cont.B,
					cont.earliest_W(), cont.became_popular_W());
				}
			}
		}
		*/
	// ------------------- set up search pattern for 4 corners of the board---------
	//Pattern p(CORNER_NW_PATTERN,19,7,7, ".................X.....X......XO.....OO..........");

	/*
	// -------------------- set up search options ----------------------------------
	SearchOptions so;
	so.fixedColor = 0;
	so.algos = algos;

	// -------------------- do pattern search --------------------------------------
	for(int i=0; i<1; i++) {
		gl->reset();
		gl->search(p, &so);
		//printf("num games: %d, num hits: %d\n", gl.size(), gl.numHits());
		// ------------------- print some statistics ------------------------------------------

		/*
		printf("Search pattern:\n");
		printf("%s\n", p.printPattern().c_str());
		*/

		/*
		printf("Continuations:\n");
		//print joseki result
		for(int y=0; y<p.sizeY; y++) {
			for(int x=0; x<p.sizeX; x++) {
				printf("%c", gl.lookupLabel(x,y));
			}
		}
		*/
		/*
		printf("\n");
		printf("Statistics:\n"); 
		printf("num hits: %d, num switched: %d, B wins: %d, W wins: %d\n", gl.num_hits, gl.num_switched, gl.Bwins, gl.Wwins);


		printf("Continuation | Black      ( B wins / W wins ) | White      (B wins / W wins) |\n");
		for(int y=0; y<p.sizeY; y++) {
			for(int x=0; x<p.sizeX; x++) {
				if (gl.lookupLabel(x,y) != '.') {
				Continuation cont = gl.lookupContinuation(x,y);
				printf("      %c      |   %3d[%3d] (    %3d /    %3d ) |   %3d[%3d] (   %3d /    %3d) | %1.1f /  %1.1f | %d | %d |  \n",
					gl.lookupLabel(x,y), cont.B, cont.tB, cont.wB, cont.lB, cont.W, cont.tW, cont.wW, cont.lW, 
					cont.wW*100.0/cont.W, cont.wB*100.0/cont.B,
					cont.earliest_W(), cont.became_popular_W());
				}
			}
		}*/
	//}


}