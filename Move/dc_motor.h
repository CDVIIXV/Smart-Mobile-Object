//
// Created by Hwiyong on 2019-08-12.
//

#ifndef SMARTMOBILEOBJECT_RASPBERRYPI_DC_MOTOR_H
#define SMARTMOBILEOBJECT_RASPBERRYPI_DC_MOTOR_H

#include <wiringPi.h>
#include <wiringPiI2C.h>

#define ADAFRUIT_MOTORHAT       0x60

#define PWM_M1_PWM      8
#define PWM_M1_IN2      9
#define PWM_M1_IN1      10
#define PWM_M2_PWM      13
#define PWM_M2_IN2      12
#define PWM_M2_IN1      11
#define PWM_M3_PWM      2
#define PWM_M3_IN2      3
#define PWM_M3_IN1      4
#define PWM_M4_PWM      7
#define PWM_M4_IN2      6
#define PWM_M4_IN1      5

#define PWM_FREQUENCY   1600.0
#define PWM_PRESCALE    0xFE

#define PWM_MODE1       0x00
#define PWM_MODE2       0x01
#define PWM_LED0_ON_L   0x06
#define PWM_LED0_ON_H   0x07
#define PWM_LED0_OFF_L  0x08
#define PWM_LED0_OFF_H  0x09

#define PWM_MODE1       0x00
#define PWM_MODE2       0x01
#define PWM_LED0_ON_L   0x06
#define PWM_LED0_ON_H   0x07
#define PWM_LED0_OFF_L  0x08
#define PWM_LED0_OFF_H  0x09

#define PWM_RESTART     0x80
#define PWM_SLEEP       0x10
#define PWM_ALLCALL     0x01
#define PWM_INVRT       0x10
#define PWM_OUTDRV      0x04

#define PWM_ALL_LED_ON_L        0xFA
#define PWM_ALL_LED_ON_H        0xFB
#define PWM_ALL_LED_OFF_L       0xFC
#define PWM_ALL_LED_OFF_H       0xFD

#define MOTOR_FORWARD   1
#define MOTOR_BACK      2
#define MOTOR_RELEASE   3

void initI2CDC();
void _setAllPWM_(int on, int off);
void _setPWM_(int pin, int on, int off);
void _setPin_(int pin, int value);

void runDCMotor(int motor, int command);
void setDCSpeed(int motor, int speed);

#endif //SMARTMOBILEOBJECT_RASPBERRYPI_DC_MOTOR_H
