#pragma config(Sensor, port4,  ringColor,      sensorVexIQ_ColorHue)
#pragma config(Motor,  motor5,          conveyorBelt,  tmotorVexIQ, PIDControl, encoder)
#pragma config(Motor,  motor6,          leftMotor,     tmotorVexIQ, PIDControl, driveLeft, encoder)
#pragma config(Motor,  motor7,          ringFlipper,   tmotorVexIQ, PIDControl, encoder)
#pragma config(Motor,  motor10,         ringArm,       tmotorVexIQ, PIDControl, encoder)
#pragma config(Motor,  motor12,         rightMotor,    tmotorVexIQ, PIDControl, reversed, driveRight, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "RobotCCompatibility.h"

#ifndef ROBOTC
#define leftMotor port1
#define rightMotor port1
#define conveyorBelt port1
#define ringFlipper port1
#define ringArm port1
#endif
volatile bool isCalibrating = false;
#define USE_COLORSENSOR
//ChA: Left Joystick, Y-axis
//ChB: Left Joystick, X-axis
//ChC: Right Joystick, X-axis
//ChD: Right Joystick, Y-axis
const int ringArmMotorSpeed = 30;
const int ringFlipperMotorSpeed = 60;
typedef struct {
	int left, right, theta;
} TMotorControl;

int min(int a, int b, int c) {
	int m = a;
	if (m > b) m = b;
	if (m > c) m = c;
	return m;
}

//https://stackoverflow.com/a/19288271
int mod(int a, int b) {
	int r = a % b;
	return r < 0 ? r + b : r;
}

//https://robotics.stackexchange.com/a/2016
void angleToThrust(int r, int theta, TMotorControl &t) {
	theta = ((theta + 180) % 360) - 180;            // normalize value to [-180, 180)
	t.theta = theta;
	if (r > 100) r = 100;                            // normalize value to [0, 100]
	int v_a = r * (45 - mod(theta, 90)) / 45;        // falloff of main motor
	int v_b = min(100, 2 * r + v_a, 2 * r - v_a);    // compensation of other motor
	if (theta < -90) {
		t.left = -v_b;
		t.right = -v_a;
	} else if (theta < 0) {
		t.left = -v_a;
		t.right = v_b;
	} else if (theta < 90) {
		t.left = v_b;
		t.right = v_a;
	} else {
		t.left = v_a;
		t.right = -v_b;
	}
}

task calibrate() {
	isCalibrating = true;
	int encoder;
	setMotorSpeed(ringFlipper, 40);
	delay(100);
	setMotorSpeed(ringFlipper, -80);
	delay(100);
	do {
		encoder = getMotorEncoder(ringFlipper);
		delay(50);
	} while (abs(getMotorEncoder(ringFlipper) - encoder) != 0);
	setMotorSpeed(ringFlipper, 0);
	delay(100);
	resetMotorEncoder(ringFlipper);
	setMotorTarget(ringFlipper, 10, 60);
	while (getMotorEncoder(ringFlipper) != 10);

	setMotorSpeed(ringArm, -80);
	delay(100);
	do {
		encoder = getMotorEncoder(ringArm);
		delay(50);
	} while (abs(getMotorEncoder(ringArm) - encoder) != 0);
	setMotorSpeed(ringArm, 0);
	delay(100);
	resetMotorEncoder(ringArm);
	setMotorTarget(ringArm, 51, 60);
	while (getMotorEncoder(ringArm) != 51);
	isCalibrating = false;
}

task wheelControl() {
	while (true) {
		int x = getJoystickValue(ChB);
		int y = getJoystickValue(ChA);
		if (x > 100) x = 100;
		else if (x < -100) x = -100;
		if (y > 100) y = 100;
		else if (y < -100) y = -100;
		//https://stackoverflow.com/questions/40363809/faster-way-to-go-from-cartesian-to-polar-in-c
		float radius, angle;
		radius = sqrt(x * x + y * y);
		angle = radiansToDegrees(atan2(y, x));
		angle -= 90;
		if (angle < 0) angle = 360 + angle;
		angle = 360 - angle;
		TMotorControl t;
		angleToThrust(radius, angle, t);
		setMotorSpeed(leftMotor, t.left);
		setMotorSpeed(rightMotor, t.right);
	}
}
bool ringDetected = false;
int ringCount = 0;
task main() {
	startTask(calibrate);
	startTask(wheelControl);
	while (true) {
#ifdef USE_COLORSENSOR
		switch(getColorName(ringColor)){
			case colorRed:
				if(ringDetected||getColorValue(ringColor)>15)break;
				ringDetected=true;
				clearTimer(T1);
				break;
			case colorGreen:
				if(ringDetected)break;
				ringDetected=true;
				clearTimer(T1);
				break;
			case colorBlue:
				if(ringDetected)break;
				ringDetected=true;
				clearTimer(T1);
				break;
		}
		displayCenteredTextLine(3, "%d",getColorValue(ringColor));
		/*displayCenteredTextLine(2, "%d", (float)getColorHue(port1)*(360.0/255.0));
		displayCenteredTextLine(3, "%d", (float)getColorSaturation(port1)*(100.0/255.0));
		displayCenteredTextLine(4, "%d", (float)getColorValue(port1)*(100.0/255.0));*/
#endif
		if (!isCalibrating) {
			if (getJoystickValue(BtnLUp)) setMotorSpeed(ringArm, ringArmMotorSpeed);
			else if (getJoystickValue(BtnLDown)) setMotorSpeed(ringArm, -ringArmMotorSpeed);
			else setMotorSpeed(ringArm, 0);
			if (getJoystickValue(BtnRUp)) {
				setMotorTarget(ringFlipper, 120, 60);
				while (getMotorEncoder(ringFlipper) < 115);
				delay(100);
				setMotorTarget(ringFlipper, 10, 60);
			}
			if (getJoystickValue(BtnEUp)) {
				setMotorTarget(ringFlipper, 10, 60);
				setMotorTarget(ringArm, 51, 60);
				while (getMotorEncoder(ringFlipper) > 15);
				while (getMotorEncoder(ringArm) < 50 || getMotorEncoder(ringArm) > 52);
			}
			static bool isFUpHeld = false;
			bool isBtnFUp = getJoystickValue(BtnFUp);
			if (isFUpHeld && !isBtnFUp)isFUpHeld = false;
			if (isBtnFUp) {
				if (!isFUpHeld) {
					if (getMotorSpeed(conveyorBelt) < 0) setMotorSpeed(conveyorBelt, 0);
					else setMotorSpeed(conveyorBelt, -50);
					isFUpHeld = true;
				}
			} else if (getJoystickValue(BtnFDown)) setMotorSpeed(conveyorBelt, 50);
			else if (getMotorSpeed(conveyorBelt) > 0) setMotorSpeed(conveyorBelt, 0);
			if(ringDetected&&time1[T1]>=900){
				ringCount++;
				if(ringCount>=4){
					setMotorSpeed(conveyorBelt,0);
					setMotorTarget(ringArm,52,60);
					delay(500);
					setMotorTarget(ringFlipper, 130, 60);
					delay(450);
					setMotorTarget(ringFlipper, 10, 60);
				}
				setMotorTarget(ringFlipper, 130, 60);
				delay(450);
				setMotorTarget(ringFlipper, 10, 60);
				if(ringCount>=3){
					playSound(soundTada);
					setMotorTarget(ringArm,70,60);
				}
				ringDetected=false;
			}
		}
	}
}
