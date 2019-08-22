#include <stdio.h>
#include "dc_motor.h"
#include "servo_motor.h"
#include "ultrasonic.h"
//#include "gyro_accel.h"
//#include "audio.h"

// DC motor
#define MOTOR_COUNT 2
#define SPEED 60
#define PARKING_SPEED 50

// servo motor
#define SERVO_PIN 1			// BCM 18
#define ANGLE_LIMIT 20
#define MID_ANGLE 100

// ultrasonic
#define ULTRASONIC_COUNT 6
#define ULTRASONIC_FORWARD_LEFT_TRIG 23		// BCM 13
#define ULTRASONIC_FORWARD_LEFT_ECHO 26		// BCM 12
#define ULTRASONIC_FORWARD_CENTER_TRIG 24	// BCM 19
#define ULTRASONIC_FORWARD_CENTER_ECHO 27	// BCM 16
#define ULTRASONIC_FORWARD_RIGHT_TRIG 29	// BCM 21
#define ULTRASONIC_FORWARD_RIGHT_ECHO 28	// BCM 20
#define ULTRASONIC_BACK_LEFT_TRIG 22		// BCM 6
#define ULTRASONIC_BACK_LEFT_ECHO 21		// BCM 5
#define ULTRASONIC_BACK_CENTER_TRIG 3	    // BCM 22
#define ULTRASONIC_BACK_CENTER_ECHO 2	    // BCM 27
#define ULTRASONIC_BACK_RIGHT_TRIG 5	    // BCM 24
#define ULTRASONIC_BACK_RIGHT_ECHO 4	    // BCM 23

#define WORK_SECOND 20

void autonomousDriving(int workSecond);
void setHandle(int angle);
void goForward(int speed);
void goBack(int speed);
void stop();
void workUltrasonic();

void parallelParking(int mode);
void parking(int isParallel);

int motor[MOTOR_COUNT] = {0, };
int ultrasonicDistance[ULTRASONIC_COUNT] = {0, };

int main() {
    int i, angle = MID_ANGLE;

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
    for(i=0; i<MOTOR_COUNT; ++i)
        motor[i] = i+1;

    // ultrasonic
    initUltrasonic(ULTRASONIC_FORWARD_LEFT_TRIG, ULTRASONIC_FORWARD_LEFT_ECHO);
    initUltrasonic(ULTRASONIC_FORWARD_CENTER_TRIG, ULTRASONIC_FORWARD_CENTER_ECHO);
    initUltrasonic(ULTRASONIC_FORWARD_RIGHT_TRIG, ULTRASONIC_FORWARD_RIGHT_ECHO);
    initUltrasonic(ULTRASONIC_BACK_LEFT_TRIG, ULTRASONIC_BACK_LEFT_ECHO);
    initUltrasonic(ULTRASONIC_BACK_CENTER_TRIG, ULTRASONIC_BACK_CENTER_ECHO);
    initUltrasonic(ULTRASONIC_BACK_RIGHT_TRIG, ULTRASONIC_BACK_RIGHT_ECHO);

    printf("Auto Driving~\n");
    autonomousDriving(WORK_SECOND);
    parallelParking(1);
    return 0;
}

void autonomousDriving(int workSecond) {
    while(workSecond > 0) {
        workUltrasonic();
        goForward(SPEED);
        sleep(5);
        workSecond -= 5;
        goBack(SPEED);
        sleep(5);
        workSecond -= 5;
    }
}

void setHandle(int angle) {
    if(angle >= 0 + ANGLE_LIMIT && angle <= 180 - ANGLE_LIMIT)
        setAngle(SERVO_PIN, 180 - angle);
}

void goForward(int speed) {
    setDCSpeed(motor[0], 6*speed/5);
    setDCSpeed(motor[1], speed);
    for(int i=0; i<MOTOR_COUNT; ++i)
        runDCMotor(motor[i], MOTOR_FORWARD);
}

void goBack(int speed) {
    setDCSpeed(motor[0], 6*speed/5);
    setDCSpeed(motor[1], speed);
    for(int i=0; i<MOTOR_COUNT; ++i)
        runDCMotor(motor[i], MOTOR_BACK);
}

void stop() {
    for(int i=0; i<MOTOR_COUNT; ++i)
        runDCMotor(motor[i], MOTOR_RELEASE);
}

void workUltrasonic() {
    ultrasonicDistance[0] = getUltrasonicDistance(ULTRASONIC_FORWARD_LEFT_TRIG, ULTRASONIC_FORWARD_LEFT_ECHO);
    ultrasonicDistance[1] = getUltrasonicDistance(ULTRASONIC_FORWARD_CENTER_TRIG, ULTRASONIC_FORWARD_CENTER_ECHO);
    ultrasonicDistance[2] = getUltrasonicDistance(ULTRASONIC_FORWARD_RIGHT_TRIG, ULTRASONIC_FORWARD_RIGHT_ECHO);
    ultrasonicDistance[3] = getUltrasonicDistance(ULTRASONIC_BACK_LEFT_TRIG, ULTRASONIC_BACK_LEFT_ECHO);
    ultrasonicDistance[4] = getUltrasonicDistance(ULTRASONIC_BACK_CENTER_TRIG, ULTRASONIC_BACK_CENTER_ECHO);
    ultrasonicDistance[5] = getUltrasonicDistance(ULTRASONIC_BACK_RIGHT_TRIG, ULTRASONIC_BACK_RIGHT_ECHO);
}

void parallelParking(int mode) {
    parking(mode);
}

void parking(int isParallel) {
    if(isParallel == 1) {
        /*
        setHandle(MID_ANGLE + 60);
        goBack(PARKING_SPEED);
        usleep(3000 * 1000);

        setHandle(MID_ANGLE - 60);
        goBack(PARKING_SPEED);
        usleep(5000 * 1000);

        goForward(PARKING_SPEED);
        usleep(2000 * 1000);
        */
        int turnAngle = MID_ANGLE + 60;

    }
    else if(isParallel == 0) {

    }
    setHandle(MID_ANGLE);
    stop();
}
