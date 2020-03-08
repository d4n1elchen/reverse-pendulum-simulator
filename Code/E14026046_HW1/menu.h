#pragma once

#include <Windows.h>

// Define btn names
#define START		0
#define RESTART		1
#define SLIDER_P	2
#define SLIDER_I	3
#define SLIDER_D	4

class Menu
{
public:
	Menu();
	Menu(float _kp, float _ki, float _kd);
	~Menu();

	void renderMenu(float w, float h);
	int processBtn(GLuint* buf);
	void updateSliders(float dy);
	bool isStarted();
	void stop();
	float getKp();
	float getKi();
	float getKd();

private:
	void genBtn(float x, float y, char *st);
	void genSlider(float x, float y, float value, char* kst, int name);

	// Sliding bar
	int sliding;

	// Control loop gains
	float kp;
	float ki;
	float kd;

	// Start flag
	int sFlag;
};