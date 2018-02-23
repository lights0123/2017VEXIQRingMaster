#pragma config(Motor, motor12, leftMotor, tmotorVexIQ, PIDControl, encoder)
#pragma config(Motor, motor7, rightMotor, tmotorVexIQ, PIDControl, reversed, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*
   Copyright 2018	 Ben Schattinger

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

/*
 * WARNING:
 * This file is saved as a "cpp" file. This does NOT mean that it uses C++! This is because RobotC
 * uses several features of C++, while retaining most of C. For example, C has pointers, but not
 * pass-by-reference (while C++ does, via the '&' character). However, RobotC doesn't support
 * pointers, and relies on the '&' character to pass by reference. Because that doesn't exist in
 * normal C, my IDE (CLion) gives me a warning that that doesn't work. I save it as a C++ file so
 * that I can use both RobotC and CLion without errors.
 */


/* Used to allow me to use my preferred IDE (CLion) to program RobotC without syntax errors -
 * it includes function headers for RobotC-specific functions.
 */
#include "RobotCCompatibility.h"

//Also for compatibilty with other IDEs.
#ifndef ROBOTC
#define leftMotor port1
#define rightMotor port2
#define armMotorBackup port8
#define rotator port10
#define miniArm port11
#define armMotor port12
#endif


typedef enum {
	DIRECTION_UP,
	DIRECTION_DOWN
} calibrationDirection;

/**
 * This is used to keep track of the status of calibration - it locks the motors being calibrated
 * otherwise. Because I'm used to Arduino, I defined it as volatile, although it probably isn't
 * needed. This tells the compiler not to optimize it, because it can change elsewhere.
 */
volatile bool isCalibrating = false;

/**
 * This function automates the calibration of a single motor. Because I don't know where a motor
 * is currently at, I need to be able to calibrate it to ensure that consistent operation is
 * achieved during each run. This is achieved by jamming the motors into pieces that stop them
 * ("endstops"). Once no movement is detected, the motors return to their correct positions to
 * run. If that process takes longer than 2 seconds, the calibration starts over again.
 *
 * @param motor the motor to calibrate
 * @param dir the direction of the endstop
 * @param endGoal the position that the motor should move to once calibration is finished
 */
void individualCalibrate(tMotor motor, calibrationDirection dir, int endGoal) {
	int encoder;

	/**
	 * Used to store if the calibration process was successful or not. If calibration failed,
	 * this variable is kept as false, and the process repeats.
	 */
	bool calibrateSuccessful = false;

	while (!calibrateSuccessful) {

		//Bring the motor out to avoid getting stuck
		setMotorSpeed(motor, dir == DIRECTION_DOWN ? 40 : -40);
		delay(300);
		//Stop it
		setMotorSpeed(motor, 0);
		delay(200);
		//Bring it back towards the endstop
		setMotorSpeed(motor, dir == DIRECTION_DOWN ? -80 : 80);
		delay(100);
		//Wait for the motor to stop (i.e. hit something)
		do {
			encoder = getMotorEncoder(motor);
			delay(50);
			/* Get the current encoder position, then wait 50ms. If the motor didn't move in that
			 * time, end the loop.
			 */
		} while (abs(getMotorEncoder(motor) - encoder) != 0);
		setMotorSpeed(motor, 0);
		delay(100);
		/* Reset the motor encoder, because it's at its correct 0 degree position,
		 * in which everything is based off of
		 */
		resetMotorEncoder(motor);
		//Go to the correct position used for the program
		setMotorTarget(motor, endGoal, dir == DIRECTION_DOWN ? -60 : 60);
		clearTimer(T2);
		//Wait until the motor is at the correct position
		while (true) {
			if (getMotorEncoder(motor) == endGoal) {
				calibrateSuccessful = true;
				break;

				// If at least 2 seconds have passed, restart calibration: it somehow failed.
			} else if (time1[T2] > 2000) break;
		}
	}
}
/**
 * Calibrate all of the motors. Currently, only one motor is calibrated, which are the miniArms.
 * See the individualCalibrate() function above for further explanation.
 */
task calibrate() {
	isCalibrating = true;
	isCalibrating = false;
}

/**
 * Gets the minimum of 3 values.
 *
 * @param[in]	a
 * @param[in]	b
 * @param[in]	c
 * @returns		The lowest value of a, b, and c
 */
int min(int a, int b, int c) {
	int m = a;
	if (m > b) m = b;
	if (m > c) m = c;
	return m;
}

/**
 * https://stackoverflow.com/a/19288271
 * The modulus operator in Python is different than C - C doesn't work with negative numbers, while Python does.
 *
 * @param[in]	a
 * @param[in]	b
 * @returns		a modulus b
 */
int mod(int a, int b) {
	int r = a % b;
	return r < 0 ? r + b : r;
}

typedef struct {                //This is used to pass data about the individual motors
	int left, right;
} TMotorControl;

/**
 * This is used to convert the joystick's location to speeds for the motor. I originally got this code from
 * https://robotics.stackexchange.com/a/2016 which is in Python, so I turned it into C++. See wheelControl() for
 * more details.
 *
 * @param[in]	r		The radius of the circle that the joystick is at (in the Polar coordinate system)
 * @param[in]	theta	The theta of the circle that the joystick is at (in the Polar coordinate system)
 * @param[out]	t		The TMotorControl struct to put data in (because RobotC doesn't support returning structs)
 */
void angleToThrust(int r, int theta, TMotorControl &t) {
	theta = ((theta + 180) % 360) - 180;            // normalize value to [-180, 180)
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
/* Vex Joystick Channels:
 *
 * ChA: Left Joystick, Y-axis
 * ChB: Left Joystick, X-axis
 * ChC: Right Joystick, X-axis
 * ChD: Right Joystick, Y-axis
 */
/**
 * A Brief overview of what happens here:
 *
 * When driving the robot, it is ideal to just use one joystick: pushing forwards goes forward, backwards back, etc.
 * However, this isn't easy to do. What I have done in the past is see which general direction is greater, but this
 * only allows the four cardinal directions. I found a method online (https://robotics.stackexchange.com/a/2016) to
 * convert the X & Y coordinates of the joystick, and convert them to speeds for the left & right motors.
 */
task wheelControl() {
	while (true) {
		int x = getJoystickValue(ChB);
		int y = getJoystickValue(ChA);
		//Ensure that the joystick positions are within range
		if (x > 100) x = 100;
		else if (x < -100) x = -100;
		if (y > 100) y = 100;
		else if (y < -100) y = -100;
		//https://stackoverflow.com/questions/40363809/faster-way-to-go-from-cartesian-to-polar-in-c
		float radius, angle;
		radius = sqrt(x * x + y * y);
		angle = radiansToDegrees(atan2(y, x));
		angle -= 90;
		//Correct for the expected input of angleToThrust()
		if (angle < 0) angle = 360 + angle;
		angle = 360 - angle;

		TMotorControl t;
		angleToThrust(radius, angle, t);
		setMotorSpeed(leftMotor, t.left);
		setMotorSpeed(rightMotor, t.right);
	}
}

task main() {
	startTask(calibrate);
	startTask(wheelControl);
	while (true);
}
