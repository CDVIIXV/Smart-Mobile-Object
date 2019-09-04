//
// Created by Hwiyong on 2019-09-03.
//

// gcc -o control_bluetooth.o control_bluetooth.c dc_motor.c servo_motor.c bluetooth.c -lwiringPi -lpthread -lbluetooth

#include <stdio.h>
#include "dc_motor.h"
#include "servo_motor.h"
#include "bluetooth.h"

// DC motor
#define MOTOR_COUNT 2
#define INIT_SPEED 60
#define SPEED_GAP 5
#define SPEED_LIMIT 100

// servo motor
#define SERVO_PIN 1			// BCM 18
#define MID_ANGLE 90
#define ANGLE_LIMIT 20      // < 90
#define ANGLE_GAP 5

void *function(void *argument);
void setSpeed(int speed);
void setHandle(int angle);
void goForward(int speed);
void goBack(int speed);
void stop();

int speed = INIT_SPEED;
int angle = MID_ANGLE;
int motor[MOTOR_COUNT] = {0, };

int main() {
    int i;
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

    bluetoothConnect(function);
    return 0;
}

void *function(void *argument) {
    char buf[1024];
    char key;

    pthread_detach(pthread_self());
    int client = (int)argument;
    int movable = 1;
    while(movable)
    {
        char *recv_message = read_server(client);
        if ( recv_message == NULL ){
            printf("client disconnected\n");
            break;
        }
        key = recv_message[0];

        if (key == 'p') {
            speed = speed + SPEED_GAP > SPEED_LIMIT ? SPEED_LIMIT : speed + SPEED_GAP;
            setSpeed(speed);
        } else if (key == 'o') {
            speed = speed - SPEED_GAP < 0 ? 0 : speed - SPEED_GAP;
            setSpeed(speed);
        } else if (key == 'w') {
            goForward(speed);
        } else if (key == 's') {
            goBack(speed);
        } else if (key == 'a') {
            angle = angle - ANGLE_GAP > MID_ANGLE - ANGLE_LIMIT ? angle - ANGLE_GAP : MID_ANGLE - ANGLE_LIMIT;
            setHandle(angle);
        } else if (key == 'd') {
            angle = angle + ANGLE_GAP < MID_ANGLE + ANGLE_LIMIT ? angle + ANGLE_GAP : MID_ANGLE + ANGLE_LIMIT;
            setHandle(angle);
        } else if (key == 'q') {
            stop();
            movable = 0;
        }
    }

    printf("disconnected\n" );
    close(client);

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