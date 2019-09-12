
void initializeMotionParams();
void setSystemParams(float mass_pend, float mass_cart, float length_arm);
void RKF45();

//  State Variables
extern float displacement,
	velocity,
	theta,
	thetaLimit,
	omega, u;

// System Parameters
extern float massPendulum,
	massCart,
	lengthArm;