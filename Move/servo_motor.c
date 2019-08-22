//
// Created by Hwiyong on 2019-08-12.
//

#include "servo_motor.h"

void initServoMotor(int servoPin) {
    pinMode(servoPin, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(SERVO_PWM_RANGE);
    pwmSetClock(SERVO_PWM_CLOCK);
}

/**
 * angle 0 = 25, degree 90 = 75, degree 180 = 125
 * MID_DEGREE 75, LEFT_DEGREE 50, RIGHT_DEGREE 100
 * @param servoPin pin number
 * @param angle degree
 */
void setAngle(int servoPin, int angle) {
    if(angle >= MIN_ANGLE && angle <= MAX_ANGLE)
        pwmWrite(servoPin, (int)((float)5 / 9 * angle + 25));
}