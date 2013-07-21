#include "ARGraphicController.h"

//  The number of frames
int frameCount = 0;
float fps = 0;

//  currentTime - previousTime is the time elapsed
//  between every call of the Idle function
int currentTime = 0, previousTime = 0;

//for ar projection
GLfloat light_ambient[4] = {0.3f, 0.3f, 0.3f, 1.0f};  
GLfloat light_diffuse[4] = {1.0f,1.0f,1.0f,1.0f}; 
GLfloat light_specular[4] = {1.0f,1.0f,1.0f,1.0f}; 
GLfloat light_position[4]= {1.0f,1.0f,1.0f,0.0f};  /* Infinite light location. */

float camera_matrix[16];
GLuint textureID;
bool detectedBoard;
CvCapture *cap;
int winID;

//static variables
//volatile char ARGraphicController::board[361];
volatile bool ARGraphicController::genMove;
volatile int ARGraphicController::newMoveColor;
int ARGraphicController::assistant_mode;

GoBoard* ARGraphicController::board;
GoAssistantController* ARGraphicController::goAssistant;
Config* ARGraphicController::config;
GoBoardDetector* ARGraphicController::d;

//loading msg animation
int ARGraphicController::loadingMsg;
int currentMsgTime =0;
int prevMsgTime =0 ;

float arrow_space_height = 0.28;
float arrow_space_height_add = 0.25;


bool showAllStones;
cv::Mat frameImg;

ARGraphicController::ARGraphicController(Config* c, GoBoard* b, GoAssistantController* ass)
{
	//assign
	board = b;
	goAssistant = ass;
	config = c;
	d = new GoBoardDetector(c);

	textureID = -1;
	cap = cvCaptureFromCAM(0);
	detectedBoard = false;

	buildProjectionMatrix(camera_matrix, config->cam.intrinsic_array, config->cam.width, config->cam.height);
	genMove = false;

	

	assistant_mode = ASSISTANT_MODE::NONE;
	loadingMsg = 0;

	showAllStones = false;

	glGenTextures(1, &textureID);                  // Create The Texture

		
}

ARGraphicController::~ARGraphicController()
{
	std::cout<<"graphic controller is destructed..."<<std::endl;
	
}

void ARGraphicController::start(int argc, char *argv[])
{
	//start openGL
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(config->cam.width,config->cam.height);
    glutInitWindowPosition(100, 100);
    winID = glutCreateWindow("AR Go");
	glutDisplayFunc(&ARGraphicController::RenderSceneCB);
    glutIdleFunc(&ARGraphicController::gl_idle_func);
	
	glutKeyboardFunc(&ARGraphicController::keyFunc);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	init();

	glBindTexture(GL_TEXTURE_2D, textureID);               
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	//initialize the texture
	frameImg = cvQueryFrame(cap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameImg.cols, frameImg.rows, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, frameImg.data);

    glutMainLoop();
}
void ARGraphicController::calculateFPS()
{
    //  Increase frame count
    frameCount++;
 
    //  Get the number of milliseconds since glutInit called
    //  (or first call to glutGet(GLUT ELAPSED TIME)).
    currentTime = glutGet(GLUT_ELAPSED_TIME);
 
    //  Calculate time passed
    int timeInterval = currentTime - previousTime;
 
    if(timeInterval > 1000)
    {
        //  calculate the number of frames per second
        fps = frameCount / (timeInterval / 1000.0f);
 
        //  Set time
        previousTime = currentTime;
 
        //  Reset frame count
        frameCount = 0;

		//read move passively
		if(genMove == true && !d->handsOnBoard && d->fullBoardInScene)
		{
			std::cout<<"read board state"<<std::endl;
			char newRealBoardStones[361];
			d->readStone(newRealBoardStones);
			
			//detect only when needing to generate move
			if(board->checkNewBoardState(newRealBoardStones, board->getMoveTurnColor())){

				if(!board->addRealStone(board->newMoveIndex, board->getMoveTurnColor())){
					//not a valid move
					fprintf(stderr, "ERROR: Invalid move\n");
				}else{
					genMove = false;
				}
			}

		}
		
    }


	//also use this to calculate the loadign msg animation
	//in terms of milliseconds
	currentMsgTime = glutGet(GLUT_ELAPSED_TIME);
	if( (currentMsgTime - prevMsgTime)/300 >0){

		prevMsgTime = currentMsgTime;
		loadingMsg = (loadingMsg+1)%4;
	}


	//calculate arrow animation
	arrow_space_height_add -= 0.25/fps;
	if(arrow_space_height_add <0) arrow_space_height_add = 0.25;

	
}

void ARGraphicController::drawBackGround()
{
	// Switch to window coordinates to render
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
		glLoadIdentity();    

		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity();
		glOrtho(-1, 1, -1, 1, 0, 1);

		
		glBindTexture(GL_TEXTURE_2D,textureID);
		// Draw a textured quad	
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex2f(-1, 1);
			glTexCoord2f(1, 0); glVertex2f(1, 1);
			glTexCoord2f(1, 1); glVertex2f(1, -1);
			glTexCoord2f(0, 1); glVertex2f(-1, -1);
			
		glEnd();


		glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}



void ARGraphicController::drawBoard()
{
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		
		glLoadIdentity();
		glLoadMatrixf(camera_matrix);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		cv::Mat_<float> Tvec;
		cv::Mat_<float> rotMat;
		d->GoBoardRaux.convertTo(rotMat,CV_32F);
		d->GoBoardTaux.convertTo(Tvec ,CV_32F);
		double post_m[16] =  {rotMat(0), rotMat(3), rotMat(6), 0.0f,
							rotMat(1), rotMat(4), rotMat(7), 0.0f,
							rotMat(2), rotMat(5), rotMat(8), 0.0f,
							-Tvec(0) -0.045 , -Tvec(1) , -Tvec(2), 1.0f};
		glLoadMatrixd(post_m);
		float p[]={0,0,0};
		drawGoStone(0.01f,0.01f,0.01f,2,2,p,0);

		
		for(size_t i=0; i< 19*19;i++)
		{
			
			float p[]={-d->Board3DPoint[i+4].x ,-d->Board3DPoint[i+4].y,-d->Board3DPoint[i+4].z};

			
			if(board->virtualStones[i] == COLOR_BLACK)
				drawGoStone(config->stone.a,config->stone.b,config->stone.c,14,14,p,0);
			else if(board->virtualStones[i] == COLOR_WHITE)
				drawGoStone(config->stone.a,config->stone.b,config->stone.c,14,14,p,1);
			
			//display error showing there are wrong board state
			if(board->warningMsg != NO_WRONG_MOVE && (int)board->wrongRealStones[i]!=NO_WRONG_MOVE)
			{
				float p[]={d->Board3DPoint[i+4].x ,d->Board3DPoint[i+4].y,d->Board3DPoint[i+4].z};
				DrawSquare(p, 0.10f, HALF_TRAN_RED_COLOR);
			}

			//
			if(showAllStones)
			{
				p[0]*=-1;
				p[1]*=-1;
				p[2]*=-1;
				DrawSquare(p, 0.05f, NEW_MOVE_COLOR);
			}
			
		}

		//draw_arrow(p, 0.055, 0.2, arrow_space_height_add + arrow_space_height, NEW_MOVE_COLOR, false);
		//draw arrrow to indicate new moves
		int i = board->newMoveIndex;
		if(i!=-1){
			float p_m[]={-d->Board3DPoint[i+4].x ,-d->Board3DPoint[i+4].y,-d->Board3DPoint[i+4].z};
			draw_arrow(p_m, 0.05, 0.2, arrow_space_height_add + arrow_space_height, NEW_MOVE_COLOR, false);
		
		}

		/*
		glLineWidth(2.5);
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(15, 0, 0);
		glEnd();
		*/
		size_t s;
		//only show assistant if ...
		if(assistant_mode == goAssistant->currentMode && !goAssistant->isProcessing){
			switch(assistant_mode)
			{
				case ASSISTANT_MODE::NONE:
					break;
				case ASSISTANT_MODE::FUEGO_BOOK:

					s = (goAssistant->FuegoBookMoves)->size();
					for(size_t i=0; i<s; i++)
					{
						int index = (*goAssistant->FuegoBookMoves)[i];
						float p[]={d->Board3DPoint[index+4].x ,d->Board3DPoint[index+4].y,d->Board3DPoint[index+4].z};
						if(i==0){
							DrawSquare(p, 0.10f, HALF_TRAN_BLUE_COLOR);
						}else{
							DrawSquare(p, 0.10f, HALF_TRAN_GREEN_COLOR);
						}
					}
					break;
				case ASSISTANT_MODE::JOSEKI:
					s = (goAssistant->josekiMoves)->size();
					for(size_t i=0; i<s; i++)
					{
						int index = (*goAssistant->josekiMoves)[i].id;
						int corner = (*goAssistant->josekiMoves)[i].corner;


						float p[]={d->Board3DPoint[index+4].x ,d->Board3DPoint[index+4].y,d->Board3DPoint[index+4].z};

						DrawSquare(p, 0.10f, CORNER_JOSEKI_COLOR[corner]);


					}

					break;
				case ASSISTANT_MODE::FUEGO_MOVE:
					s = (goAssistant->FuegoBookMoves)->size();
					for(size_t i=0; i<s; i++)
					{
						int index = (*goAssistant->FuegoBookMoves)[i];
						float p[]={d->Board3DPoint[index+4].x ,d->Board3DPoint[index+4].y,d->Board3DPoint[index+4].z};
						if(i==0){
							DrawSquare(p, 0.10f, HALF_TRAN_BLUE_COLOR);
						}else{
							DrawSquare(p, 0.10f, HALF_TRAN_GREEN_COLOR);
						}
					}
					break;
				case ASSISTANT_MODE::TERRITORY:
				
					s = (goAssistant->FuegoEstimateScore)->size();
					for(size_t i=0; i<s; i++)
					{
						float score = (*goAssistant->FuegoEstimateScore)[i];
						float p[]={d->Board3DPoint[i+4].x ,d->Board3DPoint[i+4].y,d->Board3DPoint[i+4].z};
						if(score<0)
							DrawSquare(p, score*-0.20f, WHITE_COLOR);
							//DrawPoint(p, score*-18, WHITE_COLOR);
						else
							DrawSquare(p, score*0.20f, BLACK_COLOR);
							//DrawPoint(p, score*18, BLACK_COLOR);
					}
					break;

			}

		}
		drawCoordinateAxis();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW); 
}

void ARGraphicController::RenderSceneCB()
{

	calculateFPS();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	drawBackGround();
	glDisable(GL_TEXTURE_2D);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	

	if(detectedBoard)
	{
		glEnable(GL_COLOR_MATERIAL);								// enables opengl to use glColor3f to define material color
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
		drawBoard();
		glDisable(GL_COLOR_MATERIAL);
	}
	
	//glEnable(GL_COLOR_MATERIAL);								// enables opengl to use glColor3f to define material color
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);										// enables light0
		
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
		glLoadIdentity();    

		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity();

		//  Print the FPS to the window in bottom right corner
		std::string msg = "FPS: " + floatToString(fps);
		draw_text(0.64f,-0.964f, SOLID_RED_COLOR, msg);

		
		//print board status bar
		if(detectedBoard){
			draw_circle(-0.95f,-0.93f,0.05f,GREEN_COLOR );
		}else{
			msg = "Marker Not Detected!";
			draw_text(-0.90f,-0.964f, RED_COLOR, msg);
			draw_circle(-0.95f,-0.93f,0.05f, RED_COLOR);
		}


		std::string loadingString = "";
		for(int i=0; i<loadingMsg; i++)
			loadingString+=".";

		
		//print assistant mode
		switch(assistant_mode)
		{
			case ASSISTANT_MODE::NONE:
				msg = "Assistant Mode: None";
				draw_text(-0.97f,0.90f, BLUE_COLOR, msg);
				break;
			case ASSISTANT_MODE::FUEGO_BOOK:
				if(goAssistant->isProcessing)
					msg = "Assistant Mode: Opening book "+loadingString;
				else 
					msg = "Assistant Mode: Opening book";
				draw_text(-0.97f,0.90f, BLUE_COLOR, msg);
				break;
			case ASSISTANT_MODE::JOSEKI:
				if(goAssistant->isProcessing)
					msg = "Assistant Mode: Joseki" + loadingString;
				else 
					msg = "Assistant Mode: Joseki";
				draw_text(-0.97f,0.90f, BLUE_COLOR, msg);
				break;
			case ASSISTANT_MODE::FUEGO_MOVE:
				if(goAssistant->isProcessing)
					msg = "Assistant Mode: Fuego Move" + loadingString;
				else 
					msg = "Assistant Mode: Fuego Move";
				draw_text(-0.97f,0.90f, BLUE_COLOR, msg);
				break;
					break;
			case ASSISTANT_MODE::TERRITORY:
				if(goAssistant->isProcessing)
					msg = "Assistant Mode: Territory Estimation" + loadingString;
				else 
					msg = "Assistant Mode: Territory Estimation";
				draw_text(-0.97f,0.90f, BLUE_COLOR, msg);
				break;
				
		}

		//print player turns
		if(board->getMoveTurnColor() == COLOR_BLACK){
			msg = "Black's turn";
			draw_text(0.59f,0.90f, BLACK_COLOR, msg);
			draw_circle(0.55f,0.93f,0.045f,BLACK_COLOR );
		}else{
			msg = "White's turn";
			draw_text(0.59f,0.90f, WHITE_COLOR, msg);
			draw_circle(0.55f,0.93f,0.045f,WHITE_COLOR );
		}
		
		//print warning if necessary
		if(board->warningMsg != NO_WRONG_MOVE){
			
			if(board->warningMsg == ERROR_REMOVE_THIS_STONE){
				msg = "Error: Those stones are captured";
			}else if(board->warningMsg == ERROR_REAL_OVERLAPS_VIRTUAL){
				msg = "Error: Real stone overlaps with virtual";
			}else if(board->warningMsg == ERROR_OLD_REAL_STONE_MOVED){
				msg = "Error: Stone state is change";
			}else if(board->warningMsg == ERROR_MORE_THAN_ONE_NEW_MOVES){
				msg = "Error: More than one new move";
			} else if(board->warningMsg == ERROR_NEW_MOVE_WRONG_COLOR){
				msg = "Error: New stone is the wrong color";
			}     
			draw_text(-0.90f,0.814f, RED_COLOR, msg);
			draw_circle(-0.95f,0.84f,0.04f, RED_COLOR);

		}

	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();


	glutSwapBuffers();
}

void ARGraphicController::changeGen(int c)
{
	genMove = true;
	newMoveColor = c;
	
}
int tttt=0;
void ARGraphicController::gl_idle_func()
{
	
	frameImg = cvQueryFrame(cap);
	
	if (!frameImg.empty()){
		
		
		cv::Mat undistortImage;
		undistortImage = d->getUndistortImage(frameImg);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID);           

		glTexSubImage2D(GL_TEXTURE_2D, 0,0, 0,  undistortImage.cols, undistortImage.rows,GL_BGR_EXT, GL_UNSIGNED_BYTE, undistortImage.data);


		detectedBoard =d->detectMove();
		
		if(board->newMoveIsMade)
		{
			goAssistant->pushAssistantMode(assistant_mode);
			board->newMoveIsMade = false;
		}

		glutPostRedisplay();
	}else{
		std::cout<<"asdf"<<std::endl;
		boost::this_thread::sleep(boost::posix_time::milliseconds(300));
   
	}

}
void ARGraphicController::init()
{
	// These are necessary if using glTexImage2D instead of gluBuild2DMipmaps
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Enable a single OpenGL light. */
	//glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		
	glEnable(GL_LIGHTING);										// enables lighting
	glEnable(GL_LIGHT0);										// enables light0
		
	/* Use depth buffering for hidden surface elimination. */
	glEnable(GL_DEPTH_TEST);

}

void ARGraphicController::keyFunc(unsigned char key, int x, int y)
{
	
    switch (key) {
	
    case 27: //esc
        //gFinished = true;
		glutDestroyWindow (winID);
		cvDestroyAllWindows();
		cvReleaseCapture(&cap);
		delete(d);
		_exit (0);
        break;
	case 'x':
		tttt = 0;
		break;
    case 's': 
		goAssistant->showBoard();
		break;
	case 'r':
		
		if(genMove==true){
			char newRealBoardStones[361];
			d->readStone(newRealBoardStones);
			

			if(board->checkNewBoardState(newRealBoardStones, newMoveColor)){

				if(!board->addRealStone(board->newMoveIndex, newMoveColor)){
					//not a valid move
					fprintf(stderr, "ERROR: Invalid move\n");
				}else{
					genMove = false;
				}
			}
		}
	
		break;
	case 'b':
	
		if(showAllStones) showAllStones = false;
		else showAllStones = true;
		break;
	case 'o':
		//d.readStone(newMove);x
		//d.testThrehold();
		std::cout<<std::endl<<"current game state:"<<std::endl;
		for(int i=18; i>=0;i--){
			for(int j=0; j<19;j++)
			{

				if(board->virtualStones[i*19+j]==0 || board->realStones[i*19+j]==COLOR_BLACK)
					std::cout <<"B ";
				else if(board->virtualStones[i*19+j]==1 || board->realStones[i*19+j]==COLOR_WHITE)
					std::cout <<"W ";

				else if(board->virtualStones[i*19+j]==2 && board->realStones[i*19+j]==COLOR_NONE)
					std::cout <<"o ";
			}
			std::cout<<std::endl;
		}
		break;
	case 'a':
		assistant_mode = (assistant_mode+1)%ASSISTANT_MODE::NUMBER;
		//push the assistant request in queue
		goAssistant->pushAssistantMode(assistant_mode);
		break;
	case 'p':
		d->changePostMethod();
		break;
	case 't':
		//d.GoBoardTaux.convertTo(Tvec ,CV_32F);
		break;
	case '2':
		assistant_mode = ASSISTANT_MODE::FUEGO_BOOK;
		goAssistant->pushAssistantMode(assistant_mode);
		break;
	case '3':
		assistant_mode = ASSISTANT_MODE::JOSEKI;
		goAssistant->pushAssistantMode(assistant_mode);
		break;
	case '4':
		assistant_mode = ASSISTANT_MODE::FUEGO_MOVE;
		goAssistant->pushAssistantMode(assistant_mode);
		break;
	case '5':
		assistant_mode = ASSISTANT_MODE::TERRITORY;
		goAssistant->pushAssistantMode(assistant_mode);
		break;
	case '1':
		assistant_mode = ASSISTANT_MODE::NONE;
		goAssistant->pushAssistantMode(assistant_mode);
		break;
	}
}