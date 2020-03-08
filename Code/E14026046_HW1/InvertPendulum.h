
void initializeMotionParams();
void solve();

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