#include "stdafx.h"
#include "myGL.h"
#include "menu.h"
#include "InvertPendulum.h"

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

// Generate slide bar
float const width_per_char = 8.0f;

// Generate Button
void Menu::genBtn(float x, float y, char *st)
{
	int l = strlen(st);
	float w = 50;
	float h = 20;
	glColor3f(0.5f, 0.5f, 0.5f); // Color
	Sprint(x - ((float)l / 2.0f * width_per_char), y - width_per_char, st);
	glColor3f(1.0f, 1.0f, 1.0f); // Color
	glRectf(x - w, y - h, x + w, y + h);
	glLoadIdentity();
}

void Menu::genSlider(float x, float y, float value, char* kst, int name)
{
	int kstl = strlen(kst); // Display name
	char kvs[10]; // Display value

	float bw = 23.f;
	float bh = 12.f;
	float sw = 2.5f;
	float sh = 80.f;

	float by = y + 160.f * ((value - 50.f) / 100.f);

	sprintf_s(kvs, sizeof(kvs), "%.1f", value);
	int kvsl = strlen(kvs);

	glLoadIdentity();

	// text
	glLoadName(-1);
	glColor3f(0.0f, 0.0f, 0.0f); // Color
	Sprint(x - ((float)kstl / 2.0f * width_per_char), y - 110.0f, kst);
	glColor3f(1.0f, 1.0f, 1.0f); // Color
	Sprint(x - ((float)kvsl / 2.0f * width_per_char), by - width_per_char, kvs);

	// bar
	glLoadName(name);
	glColor3f(0.5f, 0.5f, 0.5f); // Color
	glRectf(x - bw, by - bh, x + bw, by + bh);

	// slider
	glLoadName(-1);
	glColor3f(0.0f, 0.0f, 0.0f); // Color
	glRectf(x - sw, y - sh, x + sw, y + sh);
}

float const line_height = 20.f;
void Menu::genInfo(float x, float y) {
	char buf[50];
	glColor3f(0.2f, 0.2f, 0.2f);

	sprintf_s(buf, sizeof(buf), "Mass of Pendulumn = %.1f (kg)", massPendulum);
	Sprint(x, y, buf);

	sprintf_s(buf, sizeof(buf), "Mass of Cart = %.1f (kg)", massCart);
	Sprint(x, y - line_height, buf);

	sprintf_s(buf, sizeof(buf), "Length of Pendulumn = %.1f (m)", lengthArm);
	Sprint(x, y - line_height * 2, buf);

	sprintf_s(buf, sizeof(buf), "Kp = %.1f, Ki = %.1f, Kd = %.1f", kp, ki, kd);
	Sprint(x, y - line_height * 3, buf);
}

// render menu
void Menu::renderMenu(float viewW, float viewH)
{
	float btnX = - viewW / 2.f + 60.f;
	float btnY = viewH / 2.f - 40.f;

	glLoadName(START);
	genBtn(btnX, btnY, "Start");

	glLoadName(RESTART);
	genBtn(btnX + 120, btnY, "Restart");

	float infoX = btnX - 40.f;
	float infoY = btnY - 50.f;

	glLoadName(-1);
	genInfo(infoX, infoY);

	float sldX = viewW / 2.f - 50.f;
	float sldY = viewH / 2.f - 100.f;

	genSlider(sldX -   0, sldY, kp, "Kp", SLIDER_P);
	genSlider(sldX -  50, sldY, ki, "Ki", SLIDER_I);
	genSlider(sldX - 100, sldY, kd, "Kd", SLIDER_D);
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