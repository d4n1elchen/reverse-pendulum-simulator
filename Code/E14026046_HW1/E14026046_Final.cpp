// E14026046_HW3.cpp : 定義主控台應用程式的進入點。
// ****** NCKU ME Computer Graphics Final Project Copyright 2015/12/27 Create by 陳建鳴, NCKU ME106 E14026046 ******
//

#include "stdafx.h"
#include "myGL.h"
#include "InvertPendulum.h"
#include "PID_v1.h"
#include <iostream>
#include <deque>

using std::cout;
using std::cin;
using std::endl;
using std::deque;

// Declare functions
// glut callback functions
void TimerFunction(int value);
void ProcessMenu(int value);
void RenderScene(void);
void SetupRC();
void SpecialKeys(int key, int x, int y);
void KeyboardKeys(unsigned char key, int x, int y);
void ChangeSize(int w, int h);
void ProcessSelection(int xPos, int yPos);
void MouseMotion(int x, int y);
void MouseKeys(int button, int state, int x, int y);
void ProcessBtn(GLuint id);

// Costume render function
void playerSence(GLfloat xRot, GLfloat yRot, GLfloat xTrans, GLfloat yTrans, GLfloat zTrans);
void DrawGround(void);
void drawCross(void);
void genBtn(float x, float y, char *st);
void genSlider(float x, float y, float by, int name);
void renderBtns();
void renderSliders();
void drawThetaBoard(float cx, float cy, float h, float w);
void drawDispBoard(float cx, float cy, float h, float w);

// Click and Drag
int startX = 0;
int startY = 0;
int viewW = 0;
int viewH = 0;

// Rotation amounts
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;

// Translate control
static GLfloat xTrans = 0.0f;
static GLfloat yTrans = 0.0f;

// Ortho range
GLfloat nRange = 300.0f;

// Stl reading
// Triangles
MyTriangle cartTri;
MyTriangle pendTri;

// VBO
unsigned int cartVBO[2];
unsigned int pendVBO[2];

// Time function
int t = 16;
int T = 0;

// flags
int sFlag = 0;

// proj mode
GLfloat oth[16];
GLfloat pers[16];

// invert pendulum

// Define btn names
#define START		0
#define RESTART		1
#define SLIDER		2

// zooms
GLfloat playerPos[3] = {0};

// PID
//Define Variables we'll be connecting to
double Setpoint = 0, Input, Output;
// Control loop gains
float k[3] = { 50.0f, 5.0f, 0.1f };
//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint, k[2], k[1], k[0], DIRECT, T);

// texture
#define GROUND_TEXTURE  0
#define CART_TEXTURE    1
#define NUM_TEXTURES    2
GLuint  textureObjects[NUM_TEXTURES];
const char *szTextureFiles[] = { "grass.tga", "cart.tga" };

// sliding bar
char* kst[] = { "Kd", "Ki", "Kp" };
int sliding[4] = { 0 };

// Light values and coordinates
GLfloat  ambientLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat  diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat  specular[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat  pos[] = { -50.0f, 50.0f, -5.0f, 1.0f };
GLfloat  specref[] = { 0.8f, 0.8f, 0.8f, 1.0f };

// Response chart
#define TOTAL_DATA_NUM 100
deque<float> thetaDatas;
deque<float> dispDatas;

// Constrains
float posMax = 50.f;
float angMax = 0.5f;

///////////////////////////////////////////////////////////////////////////////

///////////////////
// Main function //
///////////////////
int main(int argc, char* argv[])
{
	// Generate andom num
	srand(time(NULL));

	// Initialize pendulum
	initializeMotionParams();

	// Initialize PID
	Setpoint = 0;
	myPID.SetOutputLimits(-1000, 1000);
	myPID.SetMode(AUTOMATIC);
	myPID.SetControllerDirection(REVERSE);

	// Load stl files
	cout << "////////// 讀取Stl檔... //////////\n\n";
	cartTri = loadStl("cart_u.stl");
	pendTri = loadStl("pendulum.stl");
	cout << "//////////////////////////////////\n\n";

	if (cartTri.nTriangles && pendTri.nTriangles)
	{
		cout << "成大機械系 計算機圖學 Final Project by E14026046 機三丙 陳建鳴\n\n"
			 << "作業內容:\n"
			 << "  倒單擺PID控制模擬！\n\n"
			 << "按鍵說明:\n"
			 << "  滑鼠左鍵移動視角。\n"
			 << "  ↑↓←→: 移動視角。\n"
			 << "  ＷＡＳＤ: 移動玩家位置。\n\n"
			 << "選單說明:\n"
			 << "  Start: 開始模擬。\n"
			 << "  Restart: 重置運動參數。\n"
			 << "  滑條: 設定PID控制之K值。\n\n"
			 << "右鍵選單說明:\n"
			 << "  Set system parameters: 設定系統參數。\n\n";

		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutInitWindowSize(960, 600);
		glutCreateWindow("倒單擺模擬");

		// Create the Menu
		int nMainMenu;

		nMainMenu = glutCreateMenu(ProcessMenu);
		glutAddMenuEntry("Set system parameters", 1);
		glutAttachMenu(GLUT_RIGHT_BUTTON);

		glutReshapeFunc(ChangeSize);
		glutSpecialFunc(SpecialKeys);
		glutKeyboardFunc(KeyboardKeys);
		glutMotionFunc(MouseMotion);
		//glutPassiveMotionFunc(MouseMotion);
		glutMouseFunc(MouseKeys);
		glutDisplayFunc(RenderScene);

		SetupRC();
		glutMainLoop();
	}
	else
	{
		cout << "匯入模型失敗!!\n\n";
		system("PAUSE");
		return 1;
	}



	return 0;
}
///////////////////

//////////////////////////
// Called to draw scene //
//////////////////////////
void RenderScene(void)
{
	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// change proj mode
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixf(oth);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	renderBtns();
	renderSliders();
	drawCross();

	char buf[50];
	glColor3f(0.2f, 0.2f, 0.2f);
	sprintf_s(buf, sizeof(buf), "Mass of Pendulumn = %.1f (kg)", massPendulum);
	Sprint(-viewW / 2.f, viewH / 2.f - 80.f, buf);
	sprintf_s(buf, sizeof(buf), "Mass of Cart = %.1f (kg)", massCart);
	Sprint(-viewW / 2.f, viewH / 2.f - 100.f, buf);
	sprintf_s(buf, sizeof(buf), "Length of Pendulumn = %.1f (m)", lengthArm);
	Sprint(-viewW / 2.f, viewH / 2.f - 120.f, buf);
	sprintf_s(buf, sizeof(buf), "Kp = %.1f, Ki = %.1f, Kd = %.1f", k[2], k[1], k[0]);
	Sprint(-viewW / 2.f, viewH / 2.f - 140.f, buf);

	// change proj mode
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixf(pers);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	playerSence(xRot, yRot, playerPos[0], playerPos[1], playerPos[2]);
	glTranslatef(0.f, -10.f, 0.f);

	glPushMatrix(); //畫光源
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glPopMatrix();

	// Draw the ground
	glColor3ub(0, 104, 10); // Color	
	DrawGround();
	
	glPushMatrix();
	{
		glTranslatef(0.f, 0.f, -60.f);
		drawThetaBoard(-17.5f, 15.f, 15.f, 30.f);
		drawDispBoard(17.5f, 15.f, 15.f, 30.f);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(10 * displacement, 0.f, -40.f);

		//////////
		// 單擺 //
		glPushMatrix();
		glColor3f(0.5f, 0.5f, 0.5f); // Color
		glTranslatef(0, 5.f + 1.f, 0);
		glRotatef(-theta * 180.f / 3.14159f, 0.0f, 0.0f, 1.0f);
		glTranslatef(0, pendTri.cent[1] - 1.f, 0);
		renderVBO(pendVBO, pendTri.nTriangles, pendTri.cent);
		glPopMatrix();
		/////////

		//////////
		// 車車 //
		glPushMatrix();
		glColor3f(0.5f, 0.5f, 0.5f); // Color
		glTranslatef(0, cartTri.cent[1], 0);
		renderVBO(cartVBO, cartTri.nTriangles, cartTri.cent);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureObjects[CART_TEXTURE]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTranslatef(0, 0, 1.f);
		drawCube(5.f, 2.5f, 10.f, 5.f, 5.f, true);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
		/////////
	}
	glPopMatrix();
		
	// Flush drawing commands
	glutSwapBuffers();
}
//////////////////////////

// This function does any needed initialization on the rendering context. 
void SetupRC()
{
	// Variable for texture setting
	GLbyte *pBytes;
	GLint iWidth, iHeight, iComponents;
	GLenum eFormat;

	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	glEnable(GL_CULL_FACE);		// Do not calculate inside of jet

	// Enable lighting
	glEnable(GL_LIGHTING);

	// Setup and enable light 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glEnable(GL_LIGHT0);

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);

	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// All materials hereafter have full specular reflectivity
	// with a high shine
	glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	glMateriali(GL_FRONT, GL_SHININESS, 128);

	// Light blue background
	glClearColor(135.f/255.f, 206.f/255.f, 250.f/255.f, 1.0f);

	// Set up texture maps
	glGenTextures(NUM_TEXTURES, textureObjects);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	for (int i = 0; i < NUM_TEXTURES; i++)
	{
		GLbyte *pBytes;
		GLint iWidth, iHeight, iComponents;
		GLenum eFormat;

		glBindTexture(GL_TEXTURE_2D, textureObjects[i]);

		// Load this texture map
		pBytes = gltLoadTGA(szTextureFiles[i], &iWidth, &iHeight, &iComponents, &eFormat);
		gluBuild2DMipmaps(GL_TEXTURE_2D, iComponents, iWidth, iHeight, eFormat, GL_UNSIGNED_BYTE, pBytes);
		free(pBytes);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	InitVBO(cartVBO, cartTri);
	InitVBO(pendVBO, pendTri);
}

// Window size change handler
void ChangeSize(int w, int h)
{
	GLfloat aspect;

	// Prevent a divide by zero
	if (h == 0)
		h = 1;

	viewW = w;
	viewH = h;

	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	// Reset projection matrix stack for ortho
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Establish clipping volume (left, right, bottom, top, near, far)
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -nRange * 2, nRange * 2);
	glGetFloatv(GL_PROJECTION_MATRIX, oth); // get matrix of oth

	// Reset projection matrix stack for perspective
	glLoadIdentity();

	aspect = (GLfloat)w / (GLfloat)h;
	gluPerspective(60.0f, aspect, 1.0f, 500.f);
	glGetFloatv(GL_PROJECTION_MATRIX, pers); // get matris of pers
	
	///// Remain for copy /////
	// change proj mode
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glMultMatrixf(oth);
	//glMultMatrixf(pers);

	// Reset Model view matrix stack
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//glTranslatef(0.0f, 0.0f, -600.0f);
	//////////////////////////
}

// Rotating animate
void TimerFunction(int value)
{
	T += t;
	Input = theta;
	myPID.Compute(T);
	u = Output;
	RKF45();

	if (thetaDatas.size() == TOTAL_DATA_NUM)
	{
		thetaDatas.pop_front();
	}

	thetaDatas.push_back(theta);

	if (dispDatas.size() == TOTAL_DATA_NUM)
	{
		dispDatas.pop_front();
	}

	dispDatas.push_back(displacement*10);

	// Redraw the scene with new coordinates
	glutPostRedisplay();

	if (sFlag && abs(theta) < angMax && abs(displacement*10) < posMax)	// If sFlag set and angular speed > 25, repeat this funciton.
		glutTimerFunc(t, TimerFunction, NULL);
	else
	{
		sFlag = 0;
		initializeMotionParams();
		T = 0;
		Output = 0;
		u = 0;
		Input = 0;
		myPID.SetMode(MANUAL);
		myPID.SetMode(AUTOMATIC);
	}
}

// Reset flags as appropriate in response to menu selections
void ProcessMenu(int value)
{
	switch (value)
	{
	case 1: // Set System parameters
		cout << "請輸入倒單擺質量(kg): ";
		cin >> massPendulum;
		cout << "請輸入車子質量(kg): ";
		cin >> massCart;
		cout << "請輸入倒單擺長度(m): ";
		cin >> lengthArm;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

// Special keys handler
void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
		xRot -= 0.1f;

	if (key == GLUT_KEY_DOWN)
		xRot += 0.1f;

	if (key == GLUT_KEY_LEFT)
		yRot -= 0.1f;

	if (key == GLUT_KEY_RIGHT)
		yRot += 0.1f;

	if (key > 356.0f)
		xRot = 0.0f;

	if (key < -1.0f)
		xRot = 355.0f;

	if (key > 356.0f)
		yRot = 0.0f;

	if (key < -1.0f)
		yRot = 355.0f;

	// Refresh the Window
	glutPostRedisplay();
}

// Keyboard keys handler
void KeyboardKeys(unsigned char key, int x, int y)
{
	float theta = -yRot*3.14159f/180.f;

	if (key == 'w' || key == 'W') 
	{
		playerPos[0] += 0.5f*sin(theta);
		playerPos[2] += 0.5f*cos(theta);
	}

	if (key == 's' || key == 'S')
	{
		playerPos[0] -= 0.5f*sin(theta);
		playerPos[2] -= 0.5f*cos(theta);
	}

	if (key == 'a' || key == 'A')
	{
		playerPos[0] += 0.5f*sin(3.14159f / 2.f + theta);
		playerPos[2] += 0.5f*cos(3.14159f / 2.f + theta);
	}

	if (key == 'd' || key == 'D')
	{
		playerPos[0] -= 0.5f*sin(3.14159f / 2.f + theta);
		playerPos[2] -= 0.5f*cos(3.14159f / 2.f + theta);;
	}

	// Refresh the Window
	glutPostRedisplay();	
}

///////////////////////////////////////////////////////////
// Present the information on which planet/sun was selected
// and displayed
void ProcessBtn(GLuint* buf)
{
	switch (buf[3])
	{
	case START: // Rotate the platform
		if (sFlag == 0)
		{
			cout << "Kp = " << k[2] << ", Ki = " << k[1] << ", Kd = " << k[0] << endl;
			sFlag = 1;
			glutTimerFunc(t, TimerFunction, NULL);
		}
		break;

	case RESTART: // Restart
		sFlag = 0;
		break;

	case SLIDER:
		if (sFlag == 0 && buf[0] > 1)
		{
			sliding[3] = 1;
			sliding[buf[4]] = 1;
		}
		break;

	default:
		break;
	}

	glutPostRedisplay();
}

///////////////////////////////////////////////////////////
// Process the selection, which is triggered by a right mouse
// click at (xPos, yPos).
#define BUFFER_LENGTH 64
void ProcessSelection(int xPos, int yPos)
{
	// Space for selection buffer
	static GLuint selectBuff[BUFFER_LENGTH];

	// Hit counter and viewport storage
	GLint hits, viewport[4];

	// Setup selection buffer
	glSelectBuffer(BUFFER_LENGTH, selectBuff);

	// Get the viewport
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Switch to projection and save the matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// Change render mode
	glRenderMode(GL_SELECT);
	glLoadIdentity();
	gluPickMatrix(xPos, viewport[3] - yPos, 1, 1, viewport);

	glMultMatrixf(oth);

	// Draw the scene
	renderBtns();
	renderSliders();

	// Collect the hits
	hits = glRenderMode(GL_RENDER);

	GLuint nErr = glGetError();
	// If a single hit occurred, display the info.
	if (hits == 1)
		ProcessBtn(selectBuff);

	// Restore the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Go back to modelview for normal rendering
	glMatrixMode(GL_MODELVIEW);
}

void MouseMotion(int x, int y)
{
	float px = 0.f;
	float py = 0.f;
	if (viewW <= viewH)
	{
		px = 2 * nRange;
		py = 2 * nRange * viewH / viewW;
	}
	else
	{
		px = 2 * nRange * viewW / viewH;
		py = 2 * nRange;
	}
		
	float dx = (x - startX)  ;
	float dy = (startY - y)  ;
	
	if (sliding[3] == 0)
	{
		if (dx < 0)
			yRot -= 1.0f;

		if (dx > 0)
			yRot += 1.0f;

		if (yRot > 356.0f)
			yRot = 0.0f;

		if (yRot < -1.0f)
			yRot = 355.0f;

		if (dy > 0)
			xRot -= 1.0f;

		if (dy < 0)
			xRot += 1.0f;

		if (xRot > 356.0f)
			xRot = 0.0f;

		if (xRot < -1.0f)
			xRot = 355.0f;
	}	

	startX = x;
	startY = y;

	for (int i = 0; i < 3; i++)
	{
		if (sliding[i] == 1)
		{
			k[i] += 0.1*dy;
			if (k[i] > 100.f) k[i] = 100.f;
			else if (k[i] < 0.f) k[i] = 0.f;
		}
	}

	// Refresh the Window
	glutPostRedisplay();
}

void MouseKeys(int button, int state, int x, int y)
{
	if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
	{
		startX = x;
		startY = y;
		ProcessSelection(x, y);
	}


	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		if ((sliding[0] + sliding[1] + sliding[2]) > 0)
		{
			myPID = PID(&Input, &Output, &Setpoint, k[2], k[1], k[0], DIRECT, T);
			myPID.SetOutputLimits(-1000, 1000);
			myPID.SetMode(AUTOMATIC);
			myPID.SetControllerDirection(REVERSE);
		}
		memset(sliding, 0, sizeof(sliding)); 
	}
}

void playerSence(GLfloat xRot, GLfloat yRot, GLfloat xTrans, GLfloat yTrans, GLfloat zTrans){
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	glTranslatef(xTrans, yTrans, zTrans);
}

// DrawGround
void DrawGround(void)
{
	GLfloat fExtent = 100.0f;
	GLfloat fStep = 1.0f;
	GLfloat y = -0.4f;
	GLint iStrip, iRun;
	GLfloat s = 0.0f;
	GLfloat t = 0.0f;
	GLfloat texStep = 1.0f / (fExtent * .075f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureObjects[GROUND_TEXTURE]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	for (iStrip = -fExtent; iStrip <= fExtent; iStrip += fStep)
	{
		t = 0.0f;
		glBegin(GL_TRIANGLE_STRIP);

		for (iRun = fExtent; iRun >= -fExtent; iRun -= fStep)
		{
			glTexCoord2f(s, t);
			glNormal3f(0.0f, 1.0f, 0.0f);   // All Point up
			glVertex3f(iStrip, y, iRun);

			glTexCoord2f(s + texStep, t);
			glNormal3f(0.0f, 1.0f, 0.0f);   // All Point up
			glVertex3f(iStrip + fStep, y, iRun);

			t += texStep;
		}
		glEnd();
		s += texStep;
	}
	glDisable(GL_TEXTURE_2D);
}

// Generate Button
void genBtn(float x, float y, char *st)
{
	int l = strlen(st);
	float w = 50;
	float h = 20;
	glColor3f(0.5f, 0.5f, 0.5f); // Color
	Sprint(x - ((float)l / 2.0f*8.0f), y - 8.0f, st);
	glColor3f(1.0f, 1.0f, 1.0f); // Color
	glRectf(x - w, y - h, x + w, y + h);
	glLoadIdentity();
}

// Generate slide bar
void genSlider(float x, float y, float by, int name)
{
	int l = strlen(kst[name]);
	char kvs[10];
	float bw = 10.f;
	float bh = 5.f;
	float sw = 2.5f;
	float sh = 80.f;

	sprintf_s(kvs, sizeof(kvs), "%.1f", k[name]);
	int ll = strlen(kvs);

	glLoadIdentity();
	// text
	glColor3f(0.9f, 0.9f, 0.9f); // Color
	Sprint(x - ((float)l / 2.0f*8.0f), y - 100.0f, kst[name]);
	Sprint(x - ((float)ll / 2.0f*8.0f), by + 10.0f, kvs);
	// slider
	glPushName(name);
	glColor3f(0.8f, 0.8f, 0.8f); // Color
	glRectf(x - bw, by - bh, x + bw, by + bh);
	glPopName();
	// bar
	glColor3f(0.5f, 0.5f, 0.5f); // Color
	glRectf(x - sw, y - sh, x + sw, y + sh);
}

// render menu
void renderBtns()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float w = viewW / 2.f - 50.f;
	float h = viewH / 2.f - 20.f;

	// Initialize the names stack
	glInitNames();
	glPushName(START);
	genBtn(-w, h, "Start");

	glLoadName(RESTART);
	genBtn(-w + 120, h, "Restart");

	glPopName();
}

// render sliding bars
void renderSliders()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float w = viewW / 2.f - 30.f;
	float h = viewH / 2.f - 80.f;

	glInitNames();
	glPushName(SLIDER);
	for (int i = 0; i < 3; i++)
	{
		genSlider(w - 30 * i, h, h + 160.f * ((k[i] - 50.f) / 100.f), i);
	}
	glPopName();
}

void drawCross()
{
	glLoadIdentity();
	glColor3f(0.5f, 0.5f, 0.5f); // Color
	glPushMatrix();
	{
		glRectf(-5.0f, -0.5f, 5.0f, 0.5f);
		glRectf(-0.5f, -5.0f, 0.5f, 5.0f);
	}
	glPopMatrix();
}

void drawThetaBoard(float cx, float cy, float h, float w)
{
	float bh = h;
	float bw = w;
	float d = 1.f;
	glPushMatrix();
	{
		glColor3ub(255, 255, 255);
		drawCube(cx, cy, bw, bh, d, false);
		glColor3ub(0, 0, 0);
		drawCube(cx, cy + bh / 2.f + d / 2.f, bw + d*2.f, d, d, false);
		drawCube(cx, cy - bh / 2.f - d / 2.f, bw + d*2.f, d, d, false);
		drawCube(cx - bw / 2.f - d / 2.f, cy, d, bh, d, false);
		drawCube(cx + bw / 2.f + d / 2.f, cy, d, bh, d, false);
		glTranslatef(0.f, 0.f, d / 2.f + 0.1f);
		plot(cx - bw / 2.f + 1.f, cy - bh / 2.f + 1.f, bw - 2.f, bh - 2.f, "Time", "Theta", TOTAL_DATA_NUM, angMax, thetaDatas);
	}
	glPopMatrix();
}

void drawDispBoard(float cx, float cy, float h, float w)
{
	float bh = h;
	float bw = w;
	float d = 1.f;
	glPushMatrix();
	{
		glColor3ub(255, 255, 255);
		drawCube(cx, cy, bw, bh, d, false);
		glColor3ub(0, 0, 0);
		drawCube(cx, cy + bh / 2.f + d / 2.f, bw + d*2.f, d, d, false);
		drawCube(cx, cy - bh / 2.f - d / 2.f, bw + d*2.f, d, d, false);
		drawCube(cx - bw / 2.f - d / 2.f, cy, d, bh, d, false);
		drawCube(cx + bw / 2.f + d / 2.f, cy, d, bh, d, false);
		glTranslatef(0.f, 0.f, d / 2.f + 0.1f);
		plot(cx - bw / 2.f + 1.f, cy - bh / 2.f + 1.f, bw - 2.f, bh - 2.f, "Time", "Displacement", TOTAL_DATA_NUM, posMax, dispDatas);
	}
	glPopMatrix();
}