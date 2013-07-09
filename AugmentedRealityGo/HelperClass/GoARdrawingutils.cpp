#include "GoARdrawingutils.h"

//source code edited from: http://www.cnblogs.com/acloud/archive/2011/12/12/DrawChess.html

void drawGoStone(GLfloat a, GLfloat b, GLfloat c, GLint nSlice, GLint nStack, float* origin, int color)
 {
	 
     const GLfloat PI = (GLfloat)(3.141592653589);                //圆周率PI
     GLfloat fZRotStep = 2.0f * PI / nStack;                        //沿着Z轴旋转的步长
     GLfloat fRange = atan(a / (b + c));                            //顶部圆弧的角度(单位为弧度)
     GLfloat R = sqrt(a * a + (b + c) * (b + c)) + c;            //大圆顶半径
 
     GLint nSlice1 = nSlice;                                        //底部纵向分片数量
     GLint nSlice2 = (GLint)(nSlice * (PI - fRange) * c / a);    //侧面纵向分片数量
     GLint nSlice3 = (GLint)(nSlice * R * fRange / a);            //顶部纵向分片数量
 
     GLfloat fStep1 = a / nSlice1;                                //顶部步长
     GLfloat fStep2 = (PI - fRange) / nSlice2;                    //侧面步长(弧度)
     GLfloat fStep3 = fRange / nSlice3;                            //顶部步长(弧度)
 
     GLfloat dr = -0.5f / (nSlice1 + nSlice2 + nSlice3);            //纹理半径增加的步长
 
     GLint i = 0, j = 0;
     for (i=0; i<nStack; i++)
     {
         GLfloat fZR = i * fZRotStep;                            //当前沿着Z轴旋转的弧度
         GLfloat fY = -sin(fZR);                                    //Z分量比率
         GLfloat fX = cos(fZR);                                    //X分量比率
         GLfloat fY1 = -sin(fZR + fZRotStep);                    //下一列的Z分量比率
         GLfloat fX1 = cos(fZR + fZRotStep);                        //下一列的X分量比率
         GLfloat rs = 0.5f;                                        //纹理半径的起点
 
        glBegin(GL_TRIANGLE_STRIP);
		
		if(color == COLOR_BLACK)
			glColor4f(0.00f,0.00f, 0.00f, 1.0f);
		else if(color == COLOR_WHITE)
			glColor4f(1.0f,1.0f, 1.0f, 1.0f);
         //bottom
		
        for (j=0; j<nSlice1; j++)
         {
             GLfloat r = fStep1 * j;
 
             //glTexCoord2f((0.5f + rs * fX), (0.5f + rs * fY));
             glNormal3f(0.0f, 0.0f,1.0f);
             glVertex3f(-(r * fX) +origin[0],-(r * fY)+origin[1],  origin[2]);
 
             //glTexCoord2f(0.5f + rs * fX1, 0.5f + rs * fY1);
             glNormal3f(0.0f, 0.0f,1.0f);
             glVertex3f(-(r * fX1)+origin[0], -(r *fY1)+origin[1],  origin[2]);
 
             rs += dr;
         }
		 
         //side

         for (j=0; j<nSlice2; j++)
         {
             GLfloat r = a + c * sin(fStep2 * j);
             GLfloat z = -(b + c - c * cos(fStep2 * j)); //-b
			z+=b;
             GLfloat nr = sin(fStep2 * j);
             GLfloat nZ = cos(fStep2 * j);
			nZ+=b;
			 
			 glNormal3f(nr * fX, nr * fY,nZ );
             glVertex3f(r * fX+origin[0],r * fY+origin[1], z+origin[2] );
			
			
			 glNormal3f(nr * fX1,  nr * fY1,nZ);
             glVertex3f(r * fX1+origin[0], r * fY1+origin[1], z+origin[2]);
             rs += dr;
         }
 
	 
       //top
		
		 for (j=0; j<=nSlice3; j++)
         {
             GLfloat r = -R * sin(fRange - j * fStep3);
             GLfloat z = -R * cos(fRange - j * fStep3);
             GLfloat nr = -sin(fRange - j * fStep3);
             GLfloat nZ = -cos(fRange - j * fStep3);
			 z+=b;
			 nZ+=b;

            // glTexCoord2f(0.5f + rs * fX, 0.5f + rs * fY);
             glNormal3f(nr * fX, nr * fY, nZ);
             glVertex3f(r * fX+origin[0],  r * fY+origin[1],z+origin[2]);
 
             //glTexCoord2f(0.5f + rs * fX1, 0.5f + rs * fY1);
             glNormal3f(nr * fX1,  nr * fY1,nZ);
             glVertex3f(r * fX1+origin[0], r * fY1+origin[1],z+origin[2]);
 
             rs += dr;
         }

         glEnd();
     }
	 glColor4f(1.f, 1.f, 1.f, 1.f);
 }

void DrawPoint(float* origin,float size, const float* color)
{
	glEnable(GL_BLEND);
	glPointSize(size);
	glBegin(GL_POINTS);
	//std::cout<<d.Board3DPoint.size()<<std::endl;
		glColor4f(color[0], color[1], color[2], color[3]);

		glVertex3f(origin[0],origin[1],origin[2]);

	glEnd();
	glColor4f(1.f, 1.f, 1.f, 1.f);
}


void drawCoordinateAxis()
{
  static float lineX[] = {0,0,0,-1,0,0};
  static float lineY[] = {0,0,0,0,-1,0};
  static float lineZ[] = {0,0,0,0,0,-1};

  glLineWidth(2);

  glBegin(GL_LINES);

  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3fv(lineX);
  glVertex3fv(lineX + 3);

  glColor3f(0.0f, 1.0f, 0.0f);
  glVertex3fv(lineY);
  glVertex3fv(lineY + 3);

  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex3fv(lineZ);
  glVertex3fv(lineZ + 3);

  glEnd();
}

void buildProjectionMatrix(float* m, float intrinsic_array[][3], int screen_width, int screen_height)
{
	float nearPlane = 0.01f; // Near clipping distance
	float farPlane = 100.0f; // Far clipping distance

	// Camera parameters
	float f_x = intrinsic_array[0][0]; // Focal length in x axis
	float f_y = intrinsic_array[1][1]; // Focal length in y axis (usually the same?)
	float c_x = intrinsic_array[0][2]; // Camera primary point x
	float c_y = intrinsic_array[1][2]; // Camera primary point y

	m[0] = -2.0f * f_x / screen_width;
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;

	m[4] = 0.0f;
	m[5] = 2.0f * f_y / screen_height;
	m[6] = 0.0f;
	m[7] = 0.0f;

	m[8] = 2.0f * c_x / screen_width - 1.0f;
	m[9] = 2.0f * c_y / screen_height - 1.0f;
	m[10] = -( farPlane + nearPlane) / ( farPlane - nearPlane );
	m[11] = -1.0f;

	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = -2.0f * farPlane * nearPlane / ( farPlane - nearPlane );
	m[15] = 0.0f;
}


void draw_circle(float x, float y, float radius, const float* color) { 

	int i;
	int triangleAmount = 20; //# of triangles used to draw circle
	//GLfloat radius = 0.8f; //radius
	GLfloat twicePi = 2.0f * M_PI;
		
	glBegin(GL_TRIANGLE_FAN);
		//glColor3f(0,0, 1.0f);
		glColor4f(color[0], color[1], color[2], color[3]);
		glVertex2f(x, y); // center of circle
		for(i = 0; i <= triangleAmount;i++) {
			glVertex2f(
			x + (radius * cos(i * twicePi / triangleAmount)) *480/640,
			y + (radius * sin(i * twicePi / triangleAmount))
			);
		}
	glEnd();
	glColor4f(1.f, 1.f, 1.f, 1.f);
}

//the origin points is inverted in the function
void DrawSquare(float* origin, float size, const float* color)
{
	//draw squaure
	glBegin(GL_POLYGON);
		glColor4f(color[0], color[1], color[2], color[3]);
		glVertex3f( -1*(origin[0]-size/2), -1*(origin[1]-size/2), -origin[2]);
		glVertex3f(-1*(origin[0]-size/2), -1*(origin[1]+size/2), -origin[2]);
		glVertex3f(-1*(origin[0]+size/2), -1*(origin[1]+size/2), -origin[2]);
		glVertex3f(-1*(origin[0]+size/2), -1*(origin[1]-size/2), -origin[2]);
	glEnd();
	glColor4f(1.f, 1.f, 1.f, 1.f);

}

void draw_text(float x, float y, const float* color, std::string msg)
{
	glColor4f(color[0], color[1], color[2], color[3]);
	glRasterPos2f(x, y);
	//
	
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)msg.c_str());
	glColor4f(1.f, 1.f, 1.f, 1.f);
}