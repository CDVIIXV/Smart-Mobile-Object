#include <stdio.h>
#include "dc_motor.h"
#include "servo_motor.h"
#include "key_input.h"

// DC motor
#define MOTOR_COUNT 2
#define SPEED 60
#define PARKING_SPEED 50

// servo motor
#define SERVO_PIN 1			// BCM 18
#define ANGLE_LIMIT 20
#define MID_ANGLE 90

void setSpeed(int speed);
void setHandle(int angle);
void goForward(int speed);
void goBack(int speed);
void stop();

int motor[MOTOR_COUNT] = {0, };

int main() {
    int i, angle = MID_ANGLE, speed = SPEED, move = 1;
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

    while(move) {
        key = getch();
        switch(key) {
            case 'w' : goForward(speed); break;
            case 's' : goBack(speed); break;
            case 'a' : angle -= 5; setHandle(angle); break;
            case 'd' : angle += 5; setHandle(angle); break;
            case 'o' : speed -= 5; setSpeed(speed); break;
            case 'p' : speed += 5; setSpeed(speed); break;
            case 'q' : move = 0; stop();  break;
        }
    }
    return 0;
}

void setSpeed(int speed) {
    for(int i=0; i<MOTOR_COUNT; ++i)
        setDCSpeed(motor[i], speed);
}

void setHandle(int angle) {
    if(angle >= 0 + ANGLE_LIMIT && angle <= 180 - ANGLE_LIMIT)
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
