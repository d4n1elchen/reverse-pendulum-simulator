#include "stdafx.h"
#include "InvertPendulum.h"
#include "rkf45.hpp"

#include <cmath>

// public variables
float displacement,
velocity,
theta,
thetaLimit = 0.78,
omega;

// System Parameters
float const gravity = 9.8;
float massPendulum = 1.5,
massCart = 0.3,
lengthArm = 0.6,
momentInertia = massPendulum * lengthArm * lengthArm,
A = lengthArm + momentInertia / (massPendulum * lengthArm),
B = (massPendulum + massCart) / (massPendulum * lengthArm);

int systemOrder = 4;
// Initial Condition
float iniTheta = 0.1,
iniOmega = 0,
iniDisplacement = 0,
iniVelocity = 0;

float *z = new float[systemOrder];

// Calculation Variables
float torque,
force,
alpha,
acceleration;

// Adaptive RK45 Method Parameters
float h = 0.01;

// Control Signal
float u = 0;

// Functions
void equationMotion(float t, float y[], float yp[]);
void equationConstrain(float t, float y[], float yp[]);
float constrain(float n, float min, float max);

void initializeMotionParams() {	
	displacement = iniDisplacement;
	velocity = iniVelocity;
	theta = iniTheta;
	omega = iniOmega;

	z[0] = displacement;
	z[1] = velocity;
	z[2] = theta;
	z[3] = omega;
}

void setSystemParams(float mass_pend, float mass_cart, float length_arm) {
	massPendulum = mass_pend;
	massCart = mass_cart;
	lengthArm = length_arm;
}

void equationMotion(float t, float y[], float yp[]) {

	force = A * (u + pow(y[3], 2) * sin(y[2])) - 0.5 * gravity * sin(2 * y[2]);
	acceleration = force / (A * B - pow(cos(y[2]), 2));

	torque = gravity * B * sin(y[2]) - 0.5 * pow(y[3], 2) * sin(2 * y[2]) - u * cos(y[2]);
	alpha = torque / (A * B - pow(cos(y[2]), 2));

	yp[0] = y[1];
	yp[1] = acceleration;
	yp[2] = y[3];
	yp[3] = alpha;

	return;

} // float[] equationMotion(float y[])


void equationConstrain(float t, float y[], float yp[]) {

	force = A * u;
	acceleration = force / (A * B - pow(cos(y[2]), 2));

	torque = -u * cos(y[2]);
	alpha = torque / (A * B - pow(cos(y[2]), 2));

	yp[0] = y[1];
	yp[1] = acceleration;
	yp[2] = y[3];
	yp[3] = alpha;

	return;

} // float[] equationConstrain(float y[])

float constrain(float n, float min, float max) {
	if (n > max)
		return max;
	else if (n < min)
		return min;
	else
		return n;
}

void RKF45() {
	float abserr = sqrt(r4_epsilon());
	int neqn = systemOrder;
	float relerr = sqrt(r4_epsilon());
	float t = 0;
	float t_out = t + h;

	int flag;
	float *y;
	float *yp;

	if (abs(z[2]) < thetaLimit) {

		y = z;
		yp = new float[neqn];
		flag = 1;

		flag = r4_rkf45(equationMotion, neqn, y, yp, &t, t_out, &relerr, abserr, flag);

		z = y;
	}

	else {

		z[2] = constrain(z[2], -thetaLimit, thetaLimit);
		z[3] = 0;

		y = z;
		yp = new float[neqn];
		flag = 1;

		flag = r4_rkf45(equationConstrain, neqn, y, yp, &t, t_out, &relerr, abserr, flag);

		z = y;

	}

	if (abs(z[2]) >= thetaLimit) {
		z[2] = constrain(z[2], -thetaLimit, thetaLimit);
		z[3] = 0;
	}

	displacement = z[0];
	velocity = z[1];
	theta = z[2];
	omega = z[3];

} // void RK45()