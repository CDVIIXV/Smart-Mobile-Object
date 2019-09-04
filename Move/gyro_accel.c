//
// Created by Hwiyong on 2019-08-13.
//

#include "gyro_accel.h"

int i2cGA;

void initI2CGA() {
    i2cGA = wiringPiI2CSetup(MPU_6050);
    wiringPiI2CWriteReg8(i2cGA,0x6B,0x00);//disable sleep mode
}

double getAccelScaled(int address) {
    int value = wiringPiI2CReadReg8(i2cGA, address);
    value <<= 8;
    value += wiringPiI2CReadReg8(i2cGA, address+1);
    if (value >= 0x8000)
        value = -(65536 - value);
    return value / 16384.0;
}

double getXRotation(double xScaled, double yScaled, double zScaled) {
    double radians;
    radians = atan2(yScaled, getDistance(xScaled, zScaled));
    return (radians * (180.0 / M_PI));
}

double getYRotation(double xScaled, double yScaled, double zScaled) {
    double radians;
    radians = atan2(xScaled, getDistance(yScaled, zScaled));
    return -(radians * (180.0 / M_PI));
}

double getDistance(double a, double b) {
    return sqrt((a*a) + (b*b));
}
