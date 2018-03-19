#pragma config(Motor, motor1, driveMotor, tmotorVexIQ, PIDControl, reversed, encoder)
#pragma config(Motor, motor2, strafeMotor, tmotorVexIQ, PIDControl, reversed, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*
   Copyright 2018 Ben Schattinger

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
#define driveMotor port1
#define strafeMotor port2
#endif

/*Vex Joystick Channels:
 *
 * ChA: Left Joystick, Y-axis
 * ChB: Left Joystick, X-axis
 * ChC: Right Joystick, X-axis
 * ChD: Right Joystick, Y-axis
 */

/**
 * The left joystick will control forwards & backwards movement, by moving the joystick forwards
 * and backwards. The right joystick will control left & right movement, by moving the joystick
 * left and right.
 */
task wheelControl() {
    while (true) {
        int straightPower = getJoystickValue(ChA);
        int strafePower = getJoystickValue(ChC);
        setMotorSpeed(driveMotor, straightPower);
        setMotorSpeed(strafeMotor, strafePower);
    }
}

task main() {
    startTask(wheelControl);

    while (true);
    }
}