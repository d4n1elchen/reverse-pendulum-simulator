// GL functions written by Daniel

#include <gl\glee.h>
#include <gl\shared\gltools.h>
#include <gl\shared\glframe.h>
#include <deque>
#pragma comment(lib, "GLee.lib" )

using std::deque;

///////////////////////////////////
// constants
///////////////////////////////////

struct MyTriangle
{
	STriangle * triArray;
	int nTriangles;
	float cent[3];
	float boundary[3][2];
};

///////////////////////////////////
// constants
///////////////////////////////////

// Define a constant for the value of PI
#define GL_PI 3.1415f


///////////////////////////////////
// functions
///////////////////////////////////

// Draw a filled circle at (cx,cy) with radius r by line segments.
void drawFilledCircle(GLfloat cx, GLfloat cy, GLfloat r, int segments);
// Texture mode
void drawFilledCircle(GLfloat cx, GLfloat cy, GLfloat r, int segments, GLboolean texture);

// Draw a cylinder at (0,0) with radius r and height h.
void drawCylinder(GLfloat r, GLfloat h, GLboolean texture);

// Draw a width*height*depth cube at (cx,cy).
void drawCube(GLfloat cx, GLfloat cy, GLfloat width, GLfloat height, GLfloat depth, GLboolean texture);

// Render the triangles read from stl file.
void renderTriangles(MyTriangle tri);
// Texture mode
void renderTriangles(MyTriangle tri, GLboolean texture);

// Load stl file and calculate the center coord.
MyTriangle loadStl(char *path);

// Initialize VBO
void InitVBO(unsigned int* VBO_index, MyTriangle tri);

// Render VBO
void renderVBO(unsigned int* VBO_index, int nTriangles, float* cent);

// Print string
void Sprint(int x, int y, char *st);

// Plot
void plot(float x, float y, float width, float height, char* xlabel, char* ylabel, float xMax, float yMax, deque<float> datas);