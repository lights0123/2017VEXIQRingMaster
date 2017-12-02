#pragma config(Sensor,	port4,		ringColor,		sensorVexIQ_ColorHue)
#pragma config(Motor,	motor5,		conveyorBelt,	tmotorVexIQ,	PIDControl,	encoder)
#pragma config(Motor,	motor6,		leftMotor,		tmotorVexIQ,	PIDControl,	driveLeft,	encoder)
#pragma config(Motor,	motor7,		ringFlipper,	tmotorVexIQ,	PIDControl,	encoder)
#pragma config(Motor,	motor10,	ringArm,		tmotorVexIQ,	PIDControl,	encoder)
#pragma config(Motor,	motor12,	rightMotor,		tmotorVexIQ,	PIDControl,	reversed,	driveRight,	encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*
 * WARNING:
 * This file is saved as a "cpp" file. This does NOT mean that it uses C++! This is because RobotC uses several
 * features of C++, while retaining most of C. For example, C has pointers, but not pass-by-reference (while C++
 * does, via the '&' character). However, RobotC doesn't support pointers, and relies on the '&' character to
 * pass by reference. Because that doesn't exist in normal C, my IDE (CLion) gives me a warning that that doesn't
 * work. I save it as a C++ file so that I can use both RobotC and CLion without errors.
 */


#include "RobotCCompatibility.h"
//Used to allow me to use my preferred IDE (CLion) to program RobotC without syntax errors

#ifndef ROBOTC                    //Also for compatibilty with other IDEs. NOTE: the fact that "port1" was used
#define leftMotor port1           //JUST as a filler, it does NOT correspond with the actual ports.
#define rightMotor port1
#define conveyorBelt port1
#define ringFlipper port1
#define ringArm port1
#define ringColor port1
#endif

/**
 * This is used to keep track of the status of calibration - it locks the motors being calibrated otherwise.
 * It probably isn't needed, but because I'm used to Arduino, I defined it as volatile. This tells the compiler not
 * to optimize it, because it can change elsewhere.
 */
volatile bool isCalibrating = false;

//Vex Joystick Channels:

//ChA: Left Joystick, Y-axis
//ChB: Left Joystick, X-axis
//ChC: Right Joystick, X-axis
//ChD: Right Joystick, Y-axis

const int ringArmMotorSpeed = 30;

typedef struct {                //This is used to pass data about the individual motors
	int left, right;
} TMotorControl;


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

/**
 * This calibrates the flipper and arm motors to their correct positions. This is achieved by jamming the motors
 * into pieces that stop them. Once no movement is detected, the motors return to their correct positions to run.
 * If that process takes longer than 2 seconds, the calibration starts over again.
 */
task calibrate() {
	isCalibrating = true;

	/**
	 * Store the current location of the selected motor's encoder. Used to keep track of when movement stops, to
	 * properly detect the endstops.
	 */
	int encoder;

	/**
	 * Used to store if the calibration process was successful or not.
	 */
	bool calibrateSuccessful = false;

	while (!calibrateSuccessful) {

		//Bring the flipper out to avoid getting stuck
		setMotorSpeed(ringFlipper, 40);
		delay(300);
		//Stop it
		setMotorSpeed(ringFlipper, 0);
		delay(200);
		//Bring it back towards the endstop
		setMotorSpeed(ringFlipper, -80);
		delay(100);
		//Wait for the motor to stop (i.e. hit something)
		do {
			encoder = getMotorEncoder(ringFlipper);
			delay(50);
			//Get the current encoder position, then wait 50ms. If the motor didn't move in that time,
		} while (abs(getMotorEncoder(ringFlipper) - encoder) != 0); //end the loop.
		setMotorSpeed(ringFlipper, 0);
		delay(100);
		//Reset the motor encoder, because it's at its correct 0 degree position,
		//in which everything is based off of
		resetMotorEncoder(ringFlipper);
		//Go to the correct position used for the program
		setMotorTarget(ringFlipper, 10, 60);
		clearTimer(T2);
		//Wait until the motor is at the correct position
		while (true) {
			if (getMotorEncoder(ringFlipper) != 10) {
				calibrateSuccessful = true;
				break;
			} else if (time1[T2] > 2000) break; // If at least 2 seconds have passed, restart calibration
		}
	}
	calibrateSuccessful = false;
	while (!calibrateSuccessful) {
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
		clearTimer(T2);
		while (true) {
			if (getMotorEncoder(ringArm) == 51) {
				calibrateSuccessful = true;
				break;
			} else if (time1[T2] > 2000) break;
		}
	}
	isCalibrating = false;
}
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

bool ringDetected = false;
int ringCount = 0;

task main() {
	startTask(calibrate);
	startTask(wheelControl);

	while (true) {
		/*
		 * This is used to detect the presence of a ring. This is used to automatically flip rings onto
		 * the ring arm, so that the driver doesn't have to worry about manually controlling that - only
		 * driving to them.
		 */
		switch (getColorName(ringColor)) {
			case colorRed:
			case colorGreen:
				datalogAddValue(0, getColorProximity(ringColor));
				if (ringDetected || getColorProximity(ringColor) < 225) break;
				ringDetected = true;            //Record that a ring was detected
				clearTimer(T1);                 //Reset the timer to time it
				break;
			case colorBlue:
				datalogAddValue(0, getColorProximity(ringColor));
				if (ringDetected || getColorProximity(ringColor) < 125) break;
				ringDetected = true;            //Record that a ring was detected
				clearTimer(T1);                 //Reset the timer to time it
			default:
				break;
		}

		//We don't want to override the calibration process - disable motors if so
		if (!isCalibrating) {
			//Manual control of the arm
			if (getJoystickValue(BtnLUp)) setMotorSpeed(ringArm, ringArmMotorSpeed);
			else if (getJoystickValue(BtnLDown)) setMotorSpeed(ringArm, -ringArmMotorSpeed);
			else setMotorSpeed(ringArm, 0);

			//Manual trigger of the ring flipper
			if (getJoystickValue(BtnRUp)) {
				setMotorTarget(ringFlipper, 120, 60);
				clearTimer(T2);
				while (getMotorEncoder(ringFlipper) < 115 && time1[T2] < 1000);
				delay(100);
				setMotorTarget(ringFlipper, 10, 60);
			}

			//Tells the flipper & arm to go to their normal spots
			if (getJoystickValue(BtnEUp)) {
				setMotorTarget(ringFlipper, 10, 60);
				setMotorTarget(ringArm, 51, 60);
				clearTimer(T2);
				while (getMotorEncoder(ringFlipper) > 15 || time1[T2] < 1000);
				clearTimer(T2);
				while ((getMotorEncoder(ringArm) < 50 || getMotorEncoder(ringArm) > 52) && time1[T2] < 1000);
			}

			//If FUp is pressed, then the conveyor belt moves forward. Then, if pressed again, it stops it.
			//Pressing FDown reverses it, and unlike FUp, stops as soon as it is released. I have to keep
			//track of the status of FUp, because otherwise, every time this loop executed, the belt would
			//toggle its status.
			static bool isFUpHeld = false;    //Static variables keep their value, even after the loop's current
			//iteration finished.
			bool isBtnFUp = getJoystickValue(BtnFUp);
			if (isFUpHeld && !isBtnFUp)isFUpHeld = false;
			if (isBtnFUp) {
				if (!isFUpHeld) {
					//If it's going forward, stop it
					if (getMotorSpeed(conveyorBelt) < 0) setMotorSpeed(conveyorBelt, 0);
					else setMotorSpeed(conveyorBelt, -50); //else, move it forward (note: the motor is reversed)
					isFUpHeld = true;
				}
			} else if (getJoystickValue(BtnFDown)) setMotorSpeed(conveyorBelt, 50);
			else if (getMotorSpeed(conveyorBelt) > 0) setMotorSpeed(conveyorBelt, 0);

			//The variable ringDetected keeps track of if a ring was detected by the color sensor. If it was,
			//and 900ms have passed, flip the ring onto the collection arm
			if (ringDetected && time1[T1] >= 900) {
				ringCount++;
				if (ringCount >= 4) {
					//The maximum capacity of the arm is 4 rings. If this is the case, we can stop the conveyor belt
					//and move the arm up.
					setMotorSpeed(conveyorBelt, 0);
					setMotorTarget(ringArm, 52, 60);
					delay(500);
					setMotorTarget(ringFlipper, 130, 60);
					delay(450);
					setMotorTarget(ringFlipper, 10, 60);
					ringCount = 0;
				} else {
					//However, usually, we will just flip it up without stopping the belt.
					setMotorTarget(ringFlipper, 130, 60);
					delay(450);
					setMotorTarget(ringFlipper, 10, 60);
					if (ringCount >= 3) {
						//After 3 rings have been collected, the belt generally gets caught on the rings
						//stored on the arm. We move the arm up to prevent this.
						setMotorTarget(ringArm, 70, 60);
						clearTimer(T2);
						while ((getMotorEncoder(ringArm) < 69 || getMotorEncoder(ringArm) > 71) && time1[T2]<1000);
					}
					ringDetected = false;
				}
			}
		}
	}
}
