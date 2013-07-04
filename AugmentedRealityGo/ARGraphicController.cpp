#include "ARGraphicController.h"

//  The number of frames
int frameCount = 0;
//  Number of frames per second
float fps = 0;

//  currentTime - previousTime is the time elapsed
//  between every call of the Idle function
int currentTime = 0, previousTime = 0;
/*
	//logitec older camera
	float intrinsic_array[3][3]=
	{{552.920601109929, 0, 320.7381303851895},
		{0, 547.0613005769327, 241.6573588058826},
		{0, 0, 1}};

	float distCoeffs_array[5] =
	{0.09425473192693991, -0.2856049801580997, -0.001101232983093059, 0.0007837444168083567, 0.1274994588642529};
	*/
	//hard code it for now
//for ar projection
GLfloat light_ambient[4] = {0.3f, 0.3f, 0.3f, 1.0f};  
GLfloat light_diffuse[4] = {1.0f,1.0f,1.0f,1.0f}; 
GLfloat light_specular[4] = {1.0f,1.0f,1.0f,1.0f}; 
GLfloat light_position[4]= {1.0f,1.0f,1.0f,0.0f};  /* Infinite light location. */

float intrinsic_array[3][3]=
	{{836.4486992622585, 0, 323.0858931708451},
	{0, 826.8277262586329, 213.3817272250332},
	{0, 0, 1}};

float distCoeffs_array[5] =
	{-0.001934653862378617, -0.01807873574764167, -0.004907823919307007, 0.001827540747833625, 0.1252985971583606};

cv::Mat intrinsic = cv::Mat(3, 3, CV_32FC1, &intrinsic_array);
cv::Mat distCoeffs = cv::Mat(5, 1, CV_32FC1, &distCoeffs_array);
GoBoardDetector d(intrinsic, distCoeffs);

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
FuegoAssistant* ARGraphicController::fuego;

ARGraphicController::ARGraphicController(int sw, int sh, GoBoard* b, FuegoAssistant* f)
{
	textureID = -1;
	detectedBoard = false;
	cap = cvCaptureFromCAM(0);
	
	
	screen_width = sw;
	screen_height = sh;
	buildProjectionMatrix(camera_matrix, intrinsic_array, screen_width, screen_height);

	genMove = false;

	board = b;
	fuego = f;

	assistant_mode = A_MODE_NONE;
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
    glutInitWindowSize(screen_width,screen_height);
    glutInitWindowPosition(100, 100);
    winID = glutCreateWindow("AR Go");
	glutDisplayFunc(&ARGraphicController::RenderSceneCB);
    glutIdleFunc(&ARGraphicController::gl_idle_func);
	
	glutKeyboardFunc(&ARGraphicController::keyFunc);
	/*
	glewInit();
    if (!GLEW_VERSION_2_0) {
        std::cout<<"OpenGL 2.0 not available"<<std::endl;
		exit(1);
    }*/
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	init();

    glutMainLoop();
	std::cout<<"oka..."<<std::endl;
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
    }
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
		//glScaled(1.0/640.0, 1.0/480.0, 1.0);
	
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
		d.GoBoardRaux.convertTo(rotMat,CV_32F);
		d.GoBoardTaux.convertTo(Tvec ,CV_32F);
		double post_m[16] =  {rotMat(0), rotMat(3), rotMat(6), 0.0f,
							rotMat(1), rotMat(4), rotMat(7), 0.0f,
							rotMat(2), rotMat(5), rotMat(8), 0.0f,
							-Tvec(0) -0.053 , -Tvec(1) , -Tvec(2), 1.0f};
		glLoadMatrixd(post_m);
		float p[]={0,0,0};
		drawGoStone(0.01,0.01,0.01,2,2,p,0);
		for(size_t i=4; i<d.Board3DPoint.size();i++)
		{
			
			float p[]={-d.Board3DPoint[i].x ,-d.Board3DPoint[i].y,-d.Board3DPoint[i].z};

			
			if(board->virtualStones[i-4] == 0)
				drawGoStone(0.055f,0.045f,0.026f,14,14,p,0);
			else if(board->virtualStones[i-4] == 1)
				drawGoStone(0.055f,0.045f,0.026f,14,14,p,1);
			
			/*
			if(board->wrongRealStones[i-4]!=0)
			{
				DrawPoint(p,5);
			}*/
		}
		switch(assistant_mode)
		{
			case A_MODE_NONE:
				break;
			case A_MODE_FUEGO_BOOK:
				
				for(size_t i=0; i<(fuego->bookMoves).size(); i++)
				{
					int index = fuego->bookMoves[i];
					float p[]={-d.Board3DPoint[index+4].x ,-d.Board3DPoint[index+4].y,-d.Board3DPoint[index+4].z};
					if(i==0)
						DrawPoint(p,7, HALF_TRAN_BLUE_COLOR);
					else
						DrawPoint(p,7, HALF_TRAN_GREEN_COLOR);
				}
				break;

		}
		//int i=22;
		//float p[]={-d.Board3DPoint_GL[i].x,-d.Board3DPoint_GL[i].y,-d.Board3DPoint_GL[i].z};
		//drawGoStone(0.05,0.04,0.023,10,10,p,1);
	
		//drawGoStone(0.06,0.11,0.023,50,50,p,1);
		//draw cooridnate


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

	glEnable(GL_COLOR_MATERIAL);								// enables opengl to use glColor3f to define material color
	glDisable(GL_LIGHTING);
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
		glLoadIdentity();    

		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity();

		//  Print the FPS to the window in bottom right corner
		std::string msg = "FPS: " + floatToString(fps);
		draw_text(0.70f,-0.96f, SOLID_RED_COLOR, msg);

		//print board status bar
		if(detectedBoard){
			draw_circle(-0.95f,-0.93f,0.05f,GREEN_COLOR );
		}else{
			draw_circle(-0.95f,-0.93f,0.05f, RED_COLOR );
		}
		

	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);


	glFlush();
	glutSwapBuffers();
}

void ARGraphicController::changeGen(int c)
{
	genMove = true;
	newMoveColor = c;
	
}

void ARGraphicController::gl_idle_func()
{
	//std::cout<<"in graphic control: a: "<<ARGoController::board.a<<std::endl;
	//std::cout<<"in graphic control: a: "<<(board->a)<<std::endl;

	cv::Mat frameImg = cvQueryFrame(cap);
	
	if (!frameImg.empty()){
		
		glGenTextures(1, &textureID);                  // Create The Texture

		glBindTexture(GL_TEXTURE_2D, textureID);               
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		cv::Mat undistortImage;
		undistortImage = d.getUndistortImage(frameImg);
		
		//GL_BGR
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameImg.cols, frameImg.rows, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, undistortImage.data);
		
		detectedBoard =d.detectMove();
		
		//release frame data
		frameImg = cv::Mat();
		glutPostRedisplay();
	}else
       Sleep(1);  // don't be a CPU hog 
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

	//glMaterial(GL_FRONT, GL_SPECULAR, specularColor);				// sets specular material color
	//glMaterialf(GL_FRONT, GL_SHININESS, 25.0f);					// sets shininess
		
	//glLight(GL_LIGHT0, GL_POSITION, lightPosition);				// sets light position
	//glLight(GL_LIGHT0, GL_SPECULAR, diffuseColor);				// sets specular light to white
	//glLight(GL_LIGHT0, GL_DIFFUSE, diffuseColor);					// sets diffuse light to white
	//glLightModel(GL_LIGHT_MODEL_AMBIENT, lModelAmbient);		// global ambient light 
		
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
		_exit (0);
        break;
    case 's': 
		fuego->showBoard();
	
		break;
	case 'r':
		
		if(genMove==true){
			
			char newRealBoardStones[361];
			d.readStone(newRealBoardStones);

			if(board->checkNewBoardState(newRealBoardStones, newMoveColor)){

				board->addRealStone(board->newMoveIndex, newMoveColor);
				genMove = false;
			}

			
		}
	
		break;
	case 'b':
		d.saveBackGroundBoard();
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
		assistant_mode = (assistant_mode+1)%NUMBER_OF_A_MODE;
		switch(assistant_mode)
		{
			case A_MODE_NONE:
				break;
			case A_MODE_FUEGO_BOOK:
				fuego->getBookPositions();
				break;

		}
		
		break;
	case 'p':
		d.changePostMethod();
		break;
	case 't':
		cv::Mat r;
		cv::Rodrigues(d.GoBoardRaux, r);
		std::cout<<r<<std::endl;
		//d.GoBoardTaux.convertTo(Tvec ,CV_32F);
		break;
	


  
	}
}