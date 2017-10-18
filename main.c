//#pragma config(Motor,  motor6,          leftMotor,     tmotorVexIQ, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
//ChA: Left Joystick, Y-axis
//ChB: Left Joystick, X-axis
//ChC: Right Joystick, X-axis
//ChD: Right Joystick, Y-axis
typedef struct{
	int left, right, theta;
} TMotorControl;

int min(int a, int b, int c){
	int m = a;
    if (m > b) m = b;
    if (m > c) m = c;
    return m;
}
//https://stackoverflow.com/a/19288271
int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}
//https://robotics.stackexchange.com/a/2016
void angleToThrust(int r, int theta, TMotorControl &t){
	theta = ((theta + 180) % 360) - 180;			// normalize value to [-180, 180)
	t.theta=theta;
	if(r > 100) r = 100;							// normalize value to [0, 100]
		int v_a = r * (45 - mod(theta,90)) / 45;		// falloff of main motor
	int v_b = min(100, 2 * r + v_a, 2 * r - v_a);	// compensation of other motor
	if(theta < -90){
		t.left = -v_b;t.right= -v_a;
	}else if (theta < 0){
		t.left = -v_a;t.right=v_b;
	}else if(theta < 90){
		t.left= v_b;t.right=v_a;
	}else{
		t.left=v_a;t.right=-v_b;
	}
}
task main()
{
	while(true){
		int x = getJoystickValue(ChB);
		int y = getJoystickValue(ChA);
		if(x > 100) x = 100;
		else if(x < -100) x = -100;
		if(y > 100) y = 100;
		else if(y < -100) y = -100;
		//https://stackoverflow.com/questions/40363809/faster-way-to-go-from-cartesian-to-polar-in-c
		float radius, angle;
		radius = sqrt( x*x + y*y);
		angle = radiansToDegrees(atan2(y, x));
		angle-=90;
		if(angle < 0) angle = 360 + angle;
		TMotorControl t;
		angleToThrust(radius,angle,t);
		writeDebugStreamLine("x: %d\ty:%d\tAngle: %d\tRadius: %d\tLeft: %d\tRight: %d\tCorrected Angle: %d",x,y, angle, radius, t.left, t.right, t.theta);
	}
}
