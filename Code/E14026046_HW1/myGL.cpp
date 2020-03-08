// GL functions written by Daniel

#include "stdafx.h"
#include "myGL.h"

using std::cout;
using std::deque;

void drawFilledCircle(GLfloat cx, GLfloat cy, GLfloat r, int segments){
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < segments; i++) {
		glVertex2f(
			cx + (r * cos(i * 2.0f * 3.1415926f / segments)),
			cy + (r * sin(i * 2.0f * 3.1415926f / segments))
			);
	}
	glEnd();
}

void drawFilledCircle(GLfloat cx, GLfloat cy, GLfloat r, int segments, GLboolean texture){
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < segments; i++) {
		if (texture) glTexCoord2f(1 + cos(i * 2.0f * 3.1415926f / segments) / 2, 1 + sin(i * 2.0f * 3.1415926f / segments) / 2);
		glVertex2f(
			cx + (r * cos(i * 2.0f * 3.1415926f / segments)),
			cy + (r * sin(i * 2.0f * 3.1415926f / segments))
			);
	}
	glEnd();
}

void drawCylinder(GLfloat r, GLfloat h, GLboolean texture) {
	//下蓋
	glFrontFace(GL_CW);
	drawFilledCircle(0, 0, r, 100, texture);
	glFrontFace(GL_CCW);

	//圓柱身
	GLUquadricObj *quadratic;
	quadratic = gluNewQuadric();
	if (texture) gluQuadricTexture(quadratic, true);
	gluCylinder(quadratic, r, r, h, 32, 32);

	//上蓋
	glTranslatef(0, 0, 5);
	drawFilledCircle(0, 0, r, 100, texture);
}

void renderTriangles(MyTriangle tri) {

	int nTriangles = tri.nTriangles;
	STriangle *triArray = tri.triArray;

	glBegin(GL_TRIANGLES);

	for (int i = 0; i < nTriangles; i++) {
		glNormal3f(triArray[i].n[0], triArray[i].n[1], triArray[i].n[2]);
		glVertex3f(triArray[i].a[0], triArray[i].a[1], triArray[i].a[2]);
		glVertex3f(triArray[i].b[0], triArray[i].b[1], triArray[i].b[2]);
		glVertex3f(triArray[i].c[0], triArray[i].c[1], triArray[i].c[2]);
	}

	// Done drawing Triangles
	glEnd();
}

void renderTriangles(MyTriangle tri, GLboolean texture) {

	float *boundary[3];
	for (int i = 0; i < 3; i++)
	{
		boundary[i] = tri.boundary[i];
	}	

	int nTriangles = tri.nTriangles;
	STriangle *triArray = tri.triArray;
	
	glBegin(GL_TRIANGLES);

	float w = boundary[0][1] - boundary[0][0];
	float h = boundary[1][1] - boundary[1][0];
	float d = boundary[2][1] - boundary[2][0];

	for (int i = 0; i < nTriangles; i++) {
		glNormal3f(triArray[i].n[0], triArray[i].n[1], triArray[i].n[2]);
		if (texture) glTexCoord2f((triArray[i].a[1] - boundary[1][0]) / h, (triArray[i].a[2] - boundary[2][0]) / d);
		glVertex3f(triArray[i].a[0], triArray[i].a[1], triArray[i].a[2]);
		if (texture) glTexCoord2f((triArray[i].b[1] - boundary[1][0]) / h, (triArray[i].b[2] - boundary[2][0]) / d);
		glVertex3f(triArray[i].b[0], triArray[i].b[1], triArray[i].b[2]);
		if (texture) glTexCoord2f((triArray[i].c[1] - boundary[1][0]) / h, (triArray[i].c[2] - boundary[2][0]) / d);
		glVertex3f(triArray[i].c[0], triArray[i].c[1], triArray[i].c[2]);
	}

	// Done drawing Triangles
	glEnd();
}

void drawCube(GLfloat cx, GLfloat cy, GLfloat width, GLfloat height, GLfloat depth, GLboolean texture) {

	GLfloat w = width / 2.f;
	GLfloat h = height / 2.f;
	GLfloat d = depth / 2.f;

	glBegin(GL_QUADS);

	GLfloat tw = 1.f / 3.f;
	GLfloat th = 1.f / 6.f;
	GLfloat td = 1.f / 3.f;
	GLfloat s0;
	GLfloat t0;

	// Front Face
	s0 = th;
	t0 = 0.f;
	glNormal3f(0.f, 0.f, 1.f);
	if (texture) glTexCoord2f(s0, t0+td);
	glVertex3f(cx - w, cy + h, d);
	if (texture) glTexCoord2f(s0, t0);
	glVertex3f(cx - w, cy - h, d);
	if (texture) glTexCoord2f(s0+tw, t0);
	glVertex3f(cx + w, cy - h, d);
	if (texture) glTexCoord2f(s0+tw, t0+td);
	glVertex3f(cx + w, cy + h, d);

	// Back Face
	s0 = th + tw;
	t0 = 1.f;
	glNormal3f(0.f, 0.f, -1.f);
	if (texture) glTexCoord2f(s0, t0-td);
	glVertex3f(cx + w, cy + h, -d);
	if (texture) glTexCoord2f(s0, t0);
	glVertex3f(cx + w, cy - h, -d);
	if (texture) glTexCoord2f(s0-tw, t0);
	glVertex3f(cx - w, cy - h, -d);
	if (texture) glTexCoord2f(s0-tw, t0-td);
	glVertex3f(cx - w, cy + h, -d);

	// Top Face
	s0 = th;
	t0 = td;
	glNormal3f(0.f, 1.f, 0.f);
	if (texture) glTexCoord2f(s0+tw, t0);
	glVertex3f(cx + w, cy + h, d);
	if (texture) glTexCoord2f(s0+tw, t0+td);
	glVertex3f(cx + w, cy + h, -d);
	if (texture) glTexCoord2f(s0, t0+td);
	glVertex3f(cx - w, cy + h, -d);
	if (texture) glTexCoord2f(s0, t0);
	glVertex3f(cx - w, cy + h, d);

	// Bottom Face
	s0 = th*2+tw;
	t0 = td;
	glNormal3f(0.f, -1.f, 0.f);
	if (texture) glTexCoord2f(s0, t0 + td);
	glVertex3f(cx - w, cy - h, d);
	if (texture) glTexCoord2f(s0, t0);
	glVertex3f(cx - w, cy - h, -d);
	if (texture) glTexCoord2f(s0 + tw, t0);
	glVertex3f(cx + w, cy - h, -d);
	if (texture) glTexCoord2f(s0 + tw, t0 + td);
	glVertex3f(cx + w, cy - h, d);

	// Left Face
	s0 = 0.f;
	t0 = td;
	glNormal3f(-1.f, 0.f, 0.f);
	if (texture) glTexCoord2f(s0 + th, t0);
	glVertex3f(cx - w, cy + h, d);
	if (texture) glTexCoord2f(s0 + th, t0 + td);
	glVertex3f(cx - w, cy + h, -d);
	if (texture) glTexCoord2f(s0, t0 + td);
	glVertex3f(cx - w, cy - h, -d);
	if (texture) glTexCoord2f(s0, t0);
	glVertex3f(cx - w, cy - h, d);

	// Right Face
	s0 = th+tw;
	t0 = td;
	glNormal3f(1.f, 0.f, 0.f);
	if (texture) glTexCoord2f(s0 + th, t0);
	glVertex3f(cx + w, cy - h, d);
	if (texture) glTexCoord2f(s0 + th, t0 + td);
	glVertex3f(cx + w, cy - h, -d);
	if (texture) glTexCoord2f(s0, t0 + td);
	glVertex3f(cx + w, cy + h, -d);
	if (texture) glTexCoord2f(s0, t0);
	glVertex3f(cx + w, cy + h, d);

	// Done drawing Triangles
	glEnd();
}

MyTriangle loadStl(char *path) {
	
	MyTriangle triangle;
	STriangle * triArray = LoadStlBinary(path, triangle.nTriangles);

	if (triArray) {
		triangle.triArray = triArray;
		float min[3] = { 1e7, 1e7, 1e7 };
		float max[3] = { -1e7, -1e7, -1e7 };
		for (int i = 0; i<triangle.nTriangles; i++)
		{
			//a
			if (triArray[i].a[0] < min[0])
				min[0] = triArray[i].a[0];
			else if (triArray[i].a[0] > max[0])
				max[0] = triArray[i].a[0];

			if (triArray[i].a[1] < min[1])
				min[1] = triArray[i].a[1];
			else if (triArray[i].a[1] > max[1])
				max[1] = triArray[i].a[1];

			if (triArray[i].a[2] < min[2])
				min[2] = triArray[i].a[2];
			else if (triArray[i].a[2] > max[2])
				max[2] = triArray[i].a[2];

			//b
			if (triArray[i].b[0] < min[0])
				min[0] = triArray[i].b[0];
			else if (triArray[i].b[0] > max[0])
				max[0] = triArray[i].b[0];

			if (triArray[i].b[1] < min[1])
				min[1] = triArray[i].b[1];
			else if (triArray[i].b[1] > max[1])
				max[1] = triArray[i].b[1];

			if (triArray[i].b[2] < min[2])
				min[2] = triArray[i].b[2];
			else if (triArray[i].b[2] > max[2])
				max[2] = triArray[i].b[2];

			//c
			if (triArray[i].c[0] < min[0])
				min[0] = triArray[i].c[0];
			else if (triArray[i].c[0] > max[0])
				max[0] = triArray[i].c[0];

			if (triArray[i].c[1] < min[1])
				min[1] = triArray[i].c[1];
			else if (triArray[i].c[1] > max[1])
				max[1] = triArray[i].c[1];

			if (triArray[i].c[2] < min[2])
				min[2] = triArray[i].c[2];
			else if (triArray[i].c[2] > max[2])
				max[2] = triArray[i].c[2];
		}
		for (int i = 0; i < 3; i++)
		{
			triangle.boundary[i][0] = min[i];
			triangle.boundary[i][1] = max[i];
		}
		triangle.cent[0] = (min[0] + max[0]) / 2.0f;
		triangle.cent[1] = (min[1] + max[1]) / 2.0f;
		triangle.cent[2] = (min[2] + max[2]) / 2.0f;
		cout << "成功匯入STL檔: " << path << " !\n"
			<< "此模型共有:" << triangle.nTriangles << "個三角網格\n"
			<< "邊界盒的中心為( " << triangle.cent[0] << " , " << triangle.cent[1] << " , " << triangle.cent[2] << " )\n\n";

		return triangle;
	}
	else{
		triangle.nTriangles = 0;
		return triangle;
	}

}

void InitVBO(unsigned int* VBO_index, MyTriangle tri)
{
	STriangle* triArray = tri.triArray;
	int nTriangles = tri.nTriangles;
	float *VBOv, *VBOn;

	VBOv = new float[nTriangles * 9];
	VBOn = new float[nTriangles * 9];
	int i;


	for (i = 0; i<nTriangles; i++)
	{

		VBOv[i * 9 + 0] = triArray[i].a[0];
		VBOv[i * 9 + 1] = triArray[i].a[1];
		VBOv[i * 9 + 2] = triArray[i].a[2];

		VBOv[i * 9 + 3] = triArray[i].b[0];
		VBOv[i * 9 + 4] = triArray[i].b[1];
		VBOv[i * 9 + 5] = triArray[i].b[2];

		VBOv[i * 9 + 6] = triArray[i].c[0];
		VBOv[i * 9 + 7] = triArray[i].c[1];
		VBOv[i * 9 + 8] = triArray[i].c[2];



		VBOn[i * 9 + 0] = triArray[i].n[0];
		VBOn[i * 9 + 1] = triArray[i].n[1];
		VBOn[i * 9 + 2] = triArray[i].n[2];

		VBOn[i * 9 + 3] = triArray[i].n[0];
		VBOn[i * 9 + 4] = triArray[i].n[1];
		VBOn[i * 9 + 5] = triArray[i].n[2];

		VBOn[i * 9 + 6] = triArray[i].n[0];
		VBOn[i * 9 + 7] = triArray[i].n[1];
		VBOn[i * 9 + 8] = triArray[i].n[2];
	}

	glGenBuffers(2, VBO_index);

	// Vertex
	glBindBuffer(GL_ARRAY_BUFFER, VBO_index[0]);
	glBufferData(GL_ARRAY_BUFFER, nTriangles * 9 * sizeof(float), VBOv, GL_STATIC_DRAW);

	Sleep(200);
	// Normal
	glBindBuffer(GL_ARRAY_BUFFER, VBO_index[1]);
	glBufferData(GL_ARRAY_BUFFER, nTriangles * 9 * sizeof(float), VBOn, GL_STATIC_DRAW);	
}

void renderVBO(unsigned int* VBO_index, int nTriangles, float* cent)
{
	glTranslatef(-cent[0], -cent[1], -cent[2]);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	// Vertex
	glBindBuffer(GL_ARRAY_BUFFER, VBO_index[0]);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	// Normal
	glBindBuffer(GL_ARRAY_BUFFER, VBO_index[1]);
	glNormalPointer(GL_FLOAT, 0, 0);


	glDrawArrays(GL_TRIANGLES, 0, nTriangles * 3);


	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void Sprint(int x, int y, char *st)
{
	int l, i;

	l = strlen(st);
	glRasterPos3f(x, y, 0.1f);
	for (i = 0; i < l; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, st[i]);
	}

}

void plot(float x, float y, float width, float height, char* xlabel, char* ylabel, float xMax, float yMax, deque<float> datas)
{
	float dx = width / xMax;
	float dy = height / 2.f / yMax;

	glPushMatrix();
	{
		glColor3ub(0, 0, 0);
		glBegin(GL_LINE_LOOP); // border
		{
			glVertex2f(x, y);
			glVertex2f(x + width, y);
			glVertex2f(x + width, y + height);
			glVertex2f(x, y + height);

		}
		glEnd();

		glColor3ub(0, 0, 0);
		glBegin(GL_LINES); // Setpoint
		{
			glVertex2f(x, y + height / 2.f);
			glVertex2f(x + width, y + height / 2.f);
		}
		glEnd();

		glColor3ub(255, 0, 0);
		glBegin(GL_LINE_STRIP);
		{
			for (int i = 0; i < datas.size(); i++)
			{
				glVertex2f(x + dx*i, y + height / 2.f + dy*datas[i]);
			}
		}
		glEnd();

		glColor3ub(0, 0, 0);
		Sprint(x + width / 2 - 1.f, y - 1.f, xlabel);
		Sprint(x - 1.f, y + height / 2.f, ylabel);
	}
	glPopMatrix();
}