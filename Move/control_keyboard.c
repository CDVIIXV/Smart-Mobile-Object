//
// Created by Hwiyong on 2019-08-23.
//

// gcc -o control_keyboard.o control_keyboard.c dc_motor.c servo_motor.c key_input.c -lwiringPi

#include <stdio.h>
#include "dc_motor.h"
#include "servo_motor.h"
#include "key_input.h"

// DC motor
#define MOTOR_COUNT 2
#define INIT_SPEED 60
#define SPEED_GAP 5
#define SPEED_LIMIT 100

// servo motor
#define SERVO_PIN 1			// BCM 18
#define MID_ANGLE 90
#define ANGLE_LIMIT 70      // < 90
#define ANGLE_GAP 5

void setSpeed(int speed);
void setHandle(int angle);
void goForward(int speed);
void goBack(int speed);
void stop();

int speed = INIT_SPEED;
int angle = MID_ANGLE;
int motor[MOTOR_COUNT] = {0, };

int main() {
    int i, movable = 1;
    char key;

    // mapping BCM to wpi
    if(wiringPiSetup() == -1) {
        printf("wiringPiSetup Error\n");
        return -1;
    }

    //servo motor
    initServoMotor(SERVO_PIN);
    setHandle(angle);

    // DC motor
    initI2CDC();
    for(i=0; i<MOTOR_COUNT; ++i) {
        motor[i] = i+1;
        setDCSpeed(motor[i], speed);
    }

    while(movable) {
        key = getch();
        switch(key) {
            case 'w' : goForward(speed); break;
            case 's' : goBack(speed); break;
            case 'a' :
                angle = angle - ANGLE_GAP > MID_ANGLE - ANGLE_LIMIT ? angle - ANGLE_GAP : MID_ANGLE - ANGLE_LIMIT;
                setHandle(angle); break;
            case 'd' :
                angle = angle + ANGLE_GAP < MID_ANGLE + ANGLE_LIMIT ? angle + ANGLE_GAP : MID_ANGLE + ANGLE_LIMIT;
                setHandle(angle); break;
            case 'p' :
                speed = speed + SPEED_GAP > SPEED_LIMIT ? SPEED_LIMIT : speed + SPEED_GAP;
                setSpeed(speed); break;
            case 'o' :
                speed = speed - SPEED_GAP < 0 ? 0 : speed - SPEED_GAP;
                setSpeed(speed); break;
            case 'q' : movable = 0; stop();  break;
        }
    }
    return 0;
}

void setSpeed(int speed) {
    for(int i=0; i<MOTOR_COUNT; ++i)
        setDCSpeed(motor[i], speed);
}

void setHandle(int angle) {
    if(angle >= MID_ANGLE - ANGLE_LIMIT && angle <= MID_ANGLE + ANGLE_LIMIT)
        setAngle(SERVO_PIN, 180 - angle);
}

void goForward(int speed) {
    setDCSpeed(motor[0], speed);
    setDCSpeed(motor[1], speed);
    for(int i=0; i<MOTOR_COUNT; ++i)
        runDCMotor(motor[i], MOTOR_FORWARD);
}

void goBack(int speed) {
    setDCSpeed(motor[0], speed);
    setDCSpeed(motor[1], speed);
    for(int i=0; i<MOTOR_COUNT; ++i)
        runDCMotor(motor[i], MOTOR_BACK);
}

void stop() {
    for(int i=0; i<MOTOR_COUNT; ++i)
        runDCMotor(motor[i], MOTOR_RELEASE);
}