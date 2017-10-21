//
// Created by Ben Schattinger on 10/15/17.
//

#if !defined(INC_2017VEXIQRINGMASTER_ROBOTC_H) && !defined(ROBOTC)
#define INC_2017VEXIQRINGMASTER_ROBOTC_H
#include <cstdint>
#include <string>

#define task void
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
//Remote Control
enum TVexJoysticks{
	ChA,ChB,ChC,Chd,BtnEUp,BtnEDown,BtnFUp,BtnFDown,BtnLUp,BtnLDown,BtnRUp,BtnRDown
};
extern short getJoystickValue(TVexJoysticks t);
//Display
extern void displayTextLine(const int nLineNumber, std::string sFormatString, ...);
//Debug
#define writeDebugStream printf
#define writeDebugStreamLine(format, ...) printf(format "\n", __VA_ARGS__)
#endif //INC_2017VEXIQRINGMASTER_ROBOTC_H
