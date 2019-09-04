//
// Created by Hwiyong on 2019-08-13.
//

#ifndef SMARTMOBILEOBJECT_RASPBERRYPI_GYRO_ACCEL_H
#define SMARTMOBILEOBJECT_RASPBERRYPI_GYRO_ACCEL_H

#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <math.h>

#define MPU_6050 0x60

#define ACCEL_X_WORD 0x3B
#define ACCEL_Y_WORD 0x3D
#define ACCEL_Z_WORD 0x3F

void initI2CGA();

double getAccelScaled(int address);

double getXRotation(double xScaled, double yScaled, double zScaled);

double getYRotation(double xScaled, double yScaled, double zScaled);

double getDistance(double a, double b);

#endif //SMARTMOBILEOBJECT_RASPBERRYPI_GYRO_ACCEL_H
