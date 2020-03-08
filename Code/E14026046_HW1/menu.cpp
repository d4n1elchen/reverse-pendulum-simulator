#include "stdafx.h"
#include "myGL.h"
#include "menu.h"

using namespace std;

Menu::Menu()
{
	sliding = 0;
	kp = 0;
	ki = 0;
	kd = 0;
	sFlag = 0;
}

Menu::Menu(float _kp, float _ki, float _kd)
{
	sliding = 0;
	kp = _kp;
	ki = _ki;
	kd = _kd;
	sFlag = 0;
}

Menu::~Menu()
{
}


void Menu::stop()
{
	sFlag = 0;
}

// Getters
bool Menu::isStarted() { return sFlag; }
float Menu::getKp() { return kp; }
float Menu::getKi() { return ki; }
float Menu::getKd() { return kd; }

// Generate Button
void Menu::genBtn(float x, float y, char *st)
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
float const width_per_char = 8.0f;
void Menu::genSlider(float x, float y, float value, char* kst, int name)
{
	int kstl = strlen(kst); // Display name
	char kvs[10]; // Display value

	float bw = 10.f;
	float bh = 5.f;
	float sw = 2.5f;
	float sh = 80.f;

	float by = y + 160.f * ((value - 50.f) / 100.f);

	sprintf_s(kvs, sizeof(kvs), "%.1f", value);
	int kvsl = strlen(kvs);

	glLoadIdentity();

	// text
	glLoadName(-1);
	glColor3f(0.6f, 0.6f, 0.6f); // Color
	Sprint(x - ((float)kstl / 2.0f * width_per_char), y - 100.0f, kst);
	Sprint(x - ((float)kvsl / 2.0f * width_per_char), by + 10.0f, kvs);

	// bar
	glLoadName(name);
	glColor3f(0.8f, 0.5f, 0.8f); // Color
	glRectf(x - bw, by - bh, x + bw, by + bh);

	// slider
	glLoadName(-1);
	glColor3f(0.8f, 0.8f, 0.8f); // Color
	glRectf(x - sw, y - sh, x + sw, y + sh);
}

// render menu
void Menu::renderMenu(float w, float h)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Initialize the names stack
	glInitNames();
	glPushName(START);
	genBtn(-w, h, "Start");

	glLoadName(RESTART);
	genBtn(-w + 120, h, "Restart");

	genSlider(w -   0, h - 60, kp, "Kp", SLIDER_P);
	genSlider(w -  50, h - 60, ki, "Ki", SLIDER_I);
	genSlider(w - 100, h - 60, kd, "Kd", SLIDER_D);
}

///////////////////////////////////////////////////////////
// Present the information on which planet/sun was selected
// and displayed
int Menu::processBtn(GLuint* buf)
{
	sliding = 0;
	switch (buf[3])
	{
	case START: // Rotate the platform
		if (sFlag == 0)
		{
			cout << "Kp = " << kp << ", Ki = " << ki << ", Kd = " << kd << endl;
			sFlag = 1;
		}
		break;

	case RESTART: // Restart
		sFlag = 0;
		break;

	case SLIDER_P:
	case SLIDER_I:
	case SLIDER_D:
		if (sFlag == 0)
		{
			sliding = buf[3];
		}
		break;

	default:
		break;
	}

	glutPostRedisplay();
	return buf[3];
}

float const dkdy = 0.5;
void Menu::updateSliders(float dy)
{
	switch (sliding) {
	case SLIDER_P:
		kp += dkdy*dy;
		if (kp > 100.f) kp = 100.f;
		else if (kp < 0.f) kp = 0.f;
		break;
	case SLIDER_I:
		ki += dkdy*dy;
		if (ki > 100.f) ki = 100.f;
		else if (ki < 0.f) ki = 0.f;
		break;
	case SLIDER_D:
		kd += dkdy*dy;
		if (kd > 100.f) kd = 100.f;
		else if (kd < 0.f) kd = 0.f;
		break;
	}
}