//
// Created by Hwiyong on 2019-08-12.
//

#ifndef SMARTMOBILEOBJECT_RASPBERRYPI_SERVO_MOTOR_H
#define SMARTMOBILEOBJECT_RASPBERRYPI_SERVO_MOTOR_H

#include <wiringPi.h>

// 19.2 * 10^6 Hz / pwmClock / pwmRange = 50 Hz
#define SERVO_PWM_RANGE 1000
#define SERVO_PWM_CLOCK 384

// Futaba S3003 model
#define MIN_ANGLE 0
#define MAX_ANGLE 180

void initServoMotor(int servoPin);
void setAngle(int servoPin, int angle);

#endif //SMARTMOBILEOBJECT_RASPBERRYPI_SERVO_MOTOR_H
