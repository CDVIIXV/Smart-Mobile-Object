//
// Created by Hwiyong on 2019-08-12.
//

#include "dc_motor.h"

int i2cDC;

void initI2CDC() {
    //Setup I2CDC
    i2cDC = wiringPiI2CSetup(ADAFRUIT_MOTORHAT);

    //Setup PWM
    _setAllPWM_(0, 0);
    wiringPiI2CWriteReg8(i2cDC, PWM_MODE2, PWM_OUTDRV);
    wiringPiI2CWriteReg8(i2cDC, PWM_MODE1, PWM_ALLCALL);
    delay(5);
    int mode1 = wiringPiI2CReadReg8(i2cDC, PWM_MODE1) & ~PWM_SLEEP;
    wiringPiI2CWriteReg8(i2cDC, PWM_MODE1, mode1);
    delay(5);

    //Set PWM frequency
    int prescale = (int) (25000000.0 / 4096.0 / PWM_FREQUENCY - 1.0);
    int oldmode = wiringPiI2CReadReg8(i2cDC, PWM_MODE1);
    int newmode = oldmode & 0x7F | 0x10;
    wiringPiI2CWriteReg8(i2cDC, PWM_MODE1, newmode);
    wiringPiI2CWriteReg8(i2cDC, PWM_PRESCALE, prescale);
    wiringPiI2CWriteReg8(i2cDC, PWM_MODE1, oldmode);
    delay(5);
    wiringPiI2CWriteReg8(i2cDC, PWM_MODE1, oldmode | 0x80);
}

void _setAllPWM_(int on, int off) {
    wiringPiI2CWriteReg8(i2cDC, PWM_ALL_LED_ON_L, on & 0xFF);
    wiringPiI2CWriteReg8(i2cDC, PWM_ALL_LED_ON_H, on >> 8);
    wiringPiI2CWriteReg8(i2cDC, PWM_ALL_LED_OFF_L, off & 0xFF);
    wiringPiI2CWriteReg8(i2cDC, PWM_ALL_LED_OFF_H, off >> 8);
}

void _setPWM_(int pin, int on, int off) {
    wiringPiI2CWriteReg8(i2cDC, PWM_LED0_ON_L + 4 * pin, on & 0xFF);
    wiringPiI2CWriteReg8(i2cDC, PWM_LED0_ON_H + 4 * pin, on >> 8);
    wiringPiI2CWriteReg8(i2cDC, PWM_LED0_OFF_L + 4 * pin, off & 0xFF);
    wiringPiI2CWriteReg8(i2cDC, PWM_LED0_OFF_H + 4 * pin, off >> 8);
}

void _setPin_(int pin, int value) {
    if (pin < 0 || pin > 15)
        return;

    switch (value) {
        case 0:
            _setPWM_(pin, 0, 4096);
            break;
        case 1:
            _setPWM_(pin, 4096, 0);
            break;
    }
}

void runDCMotor(int motor, int command) {
    int in1, in2;

    switch (motor) {
        case 1:
            in1 = PWM_M1_IN1;
            in2 = PWM_M1_IN2;
            break;
        case 2:
            in1 = PWM_M2_IN1;
            in2 = PWM_M2_IN2;
            break;
        case 3:
            in1 = PWM_M3_IN1;
            in2 = PWM_M3_IN2;
            break;
        case 4:
            in1 = PWM_M4_IN1;
            in2 = PWM_M4_IN2;
            break;
    }

    switch (command) {
        case MOTOR_FORWARD:
            _setPin_(in2, 0);
            _setPin_(in1, 1);
            break;
        case MOTOR_BACK:
            _setPin_(in1, 0);
            _setPin_(in2, 1);
            break;
        case MOTOR_RELEASE:
            _setPin_(in1, 0);
            _setPin_(in2, 0);
            break;
    }
}

void setDCSpeed(int motor, int speed) {
    if (speed < 0 || speed > 255)
        return;

    int pwm;
    switch (motor) {
        case 1:
            pwm = PWM_M1_PWM;
            break;
        case 2:
            pwm = PWM_M2_PWM;
            break;
        case 3:
            pwm = PWM_M3_PWM;
            break;
        case 4:
            pwm = PWM_M4_PWM;
            break;
    }
    _setPWM_(pwm, 0, speed * 16);
}