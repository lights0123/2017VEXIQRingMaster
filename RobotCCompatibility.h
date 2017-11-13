//
// Created by Ben Schattinger on 10/15/17.
//

#if !defined(INC_2017VEXIQRINGMASTER_ROBOTC_H) && !defined(ROBOTC)
#define INC_2017VEXIQRINGMASTER_ROBOTC_H

#include <cstdint>
#include <string>

#define task void
//Control
extern void delay(int d);
//Math
extern float abs(const float input);

extern float acos(const float Cosine);

extern float asin(const float Sine);

extern float atan(const float Tangent);

extern float atan2(const float y, const float x);

extern float atof(std::string str);

extern long atoi(std::string str);

extern float ceil(const float input);

extern float cos(const float fRadians);

extern float cosDegrees(const float fDegrees);

extern float degreesToRadians(const float fDegrees);

extern float exp(const float input);

extern float floor(const float input);

extern float log(const float input);

extern float log10(const float input);

extern const float PI;

extern float pow(const float base, const float exponent);

extern short radiansToDegrees(const float fRadians);

extern int16_t rand();

extern long randLong();

extern short sgn(const float input);

extern float sin(const float fRadians);

extern float sinDegrees(const float fDegrees);

extern void srand(const long nSeedValue);

extern float sqrt(const float input);
extern int abs(int in);

//Ports
enum ports {
	port1,
	port2,
	port3,
	port4,
	port5,
	port6,
	port7,
	port8,
	port9,
	port10,
	port11,
	port12
};
//Remote Control
enum TVexJoysticks {
	ChA, ChB, ChC, Chd, BtnEUp, BtnEDown, BtnFUp, BtnFDown, BtnLUp, BtnLDown, BtnRUp, BtnRDown
};

extern short getJoystickValue(TVexJoysticks t);

//Display
extern void displayTextLine(const int nLineNumber, std::string sFormatString, ...);

extern void displayCenteredTextLine(const int nLineNumber, std::string sFormatString, ...);
//Debug
#define writeDebugStream printf
#define writeDebugStreamLine(format, ...) printf(format "\n", __VA_ARGS__)

//Motors
extern void setMotorSpeed(ports port, int speed);
extern int getMotorEncoder(ports port);
extern void resetMotorEncoder(ports port);
extern void setMotorTarget(ports port, int target, int speed);
extern int getMotorSpeed(ports port);

//Sensors
enum colorSensorModes {
	colorTypeUninitialized,
	colorTypeGrayscale_Ambient,
	colorTypeGrayscale_Reflected,
	colorTypeRGB_12Colors_Ambient,
	colorTypeRGB_12Colors_Reflected,
	colorTypeRGB_Hue_Ambient,
	colorTypeRGB_Hue_Reflected,
	colorTypeRGB_Raw_Ambient,
	colorTypeRGB_Raw_Reflected
};
enum colors{
	colorNone,
	colorRedViolet,
	colorRed,
	colorDarkOrange,
	colorOrange,
	colorDarkYellow,
	colorYellow,
	colorLimeGreen,
	colorGreen,
	colorBlueGreen,
	colorBlue,
	colorDarkBlue,
	colorViolet
};
extern colors getColorName(ports port);
extern int getColorValue(ports port);
extern void setColorMode(ports port, colorSensorModes mode);
//Tasks
extern void startTask(void());
//Timers
enum timers{
	T1,
	T2,
	T3,
	T4
};
extern void clearTimer(timers timer);
int time1[4];

#endif //INC_2017VEXIQRINGMASTER_ROBOTC_H
