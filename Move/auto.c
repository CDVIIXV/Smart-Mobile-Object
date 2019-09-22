//
// Created by Hwiyong on 2019-08-23.
//

// sudo apt install libbluetooth-dev
// gcc -o auto.o auto.c dc_motor.c servo_motor.c ultrasonic.c bluetooth.c -lwiringPi -lpthread -lbluetooth

#include <stdio.h>
#include <stdlib.h>
#include "dc_motor.h"
#include "servo_motor.h"
#include "ultrasonic.h"
#include "bluetooth.h"

// DC motor
#define MOTOR_COUNT 2
#define INIT_SPEED 60
#define SPEED_GAP 5
#define PARKING_SPEED 50
#define SPEED_LIMIT 100

// servo motor
#define SERVO_PIN 1			// BCM 18
#define ANGLE_LIMIT 70
#define ANGLE_GAP 15
#define MID_ANGLE 95

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

#define CRASH_DISTANCE 30   // 30cm

void *function(void *argument);
void autonomousDriving(int forward, int right);
void setHandle(int angle);
void goForward(int speed);
void goBack(int speed);
void setSpeed(int speed);
void stop();
void workUltrasonic();

void parallelParking(int mode);
void parking(int isParallel);

int speed = INIT_SPEED;
int angle = MID_ANGLE;
int motor[MOTOR_COUNT] = {0, };
int ultrasonicDistance[ULTRASONIC_COUNT] = {0, };

int main() {
    int i;

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

    bluetoothConnect(function);
    return 0;
}

void *function(void *argument) {
    char buf[1024];
    char key;

    pthread_detach(pthread_self());
    int client = (int)argument;
    int autoMode = 1;
    while(1) {
        char *recv_message = read_server(client);
        if ( recv_message == NULL ) {
            printf("client disconnected\n");
            break;
        }
        key = recv_message[0];
        if(key == 'm')
            autoMode ^= 1;
        else if(autoMode == 1) {
            int numberCount = 0, forwardValue = 0, rightValue = 0, value;
            char *ptr = strtok(recv_message, ",");
            while (ptr != NULL) {
                value = atoi(ptr);
                switch(numberCount) {
                    case 0 : forwardValue += value; break;
                    case 1 : forwardValue -= value; break;
                    case 2 : rightValue -= value; break;
                    case 3 : rightValue += value; break;
                }
                ++numberCount;
                ptr = strtok(NULL, ",");
            }
            if(abs(forwardValue) >= abs(rightValue))
                autonomousDriving(forwardValue, rightValue);
        }
        else if(autoMode == 0){
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
                break;
            }
        }
    }
    stop();
    printf("disconnected\n");
    close(client);
    return 0;
}

void autonomousDriving(int forward, int right) {
    int scoreChart[6][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {1, -1}, {1, 0}, {1, 1}};
    int score = abs(forward) + abs(right), repeat = 0;
    while((forward != 0 || right != 0) && repeat < 100) {
        printf("%d %d\n", forward, right);
        workUltrasonic();
        int selectIndex = -1;
        for(int i=0; i<6; ++i) {
            if(((i < 3 && ultrasonicDistance[1] > CRASH_DISTANCE) || (i >= 3 && ultrasonicDistance[4] > CRASH_DISTANCE)) && (ultrasonicDistance[i] > CRASH_DISTANCE)) {
                int testScore = abs(forward + scoreChart[i][0]) + abs(right + scoreChart[i][1]);
                if(testScore < score) {
                    selectIndex = i;
                    score = testScore;
                }
            }
        }
        if(selectIndex > -1) {
            if(scoreChart[selectIndex][1] == -1) {
                setHandle(MID_ANGLE - ANGLE_GAP);
            }
            else if(scoreChart[selectIndex][1] == 1) {
                setHandle(MID_ANGLE + ANGLE_GAP);
            }
            if(scoreChart[selectIndex][0] == -1) {
                goForward(INIT_SPEED);
            }
            else if(scoreChart[selectIndex][0] == 1) {
                goBack(INIT_SPEED);
            }
            forward += scoreChart[selectIndex][0];
            right += scoreChart[selectIndex][1];
        }
        else {
            ++repeat;
            stop();
        }
        delay(50);
    }
    stop();
    //parallelParking(1);
}

void setHandle(int angle) {
    if(angle >= MID_ANGLE - ANGLE_LIMIT && angle <= MID_ANGLE + ANGLE_LIMIT)
        setAngle(SERVO_PIN, 180 - angle);
}

void goForward(int speed) {
    setSpeed(speed);
    for(int i=0; i<MOTOR_COUNT; ++i)
        runDCMotor(motor[i], MOTOR_FORWARD);
}

void goBack(int speed) {
    setSpeed(speed);
    for(int i=0; i<MOTOR_COUNT; ++i)
        runDCMotor(motor[i], MOTOR_BACK);
}

void setSpeed(int speed) {
    for(int i=0; i<MOTOR_COUNT; ++i)
        setDCSpeed(motor[i], speed);
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
        setHandle(MID_ANGLE + 75);
        goBack(PARKING_SPEED);
        usleep(1400 * 1000);
        setHandle(MID_ANGLE - 75);
        goBack(PARKING_SPEED);
        usleep(2800 * 1000);
        goForward(PARKING_SPEED);
        usleep(300 * 1000);
    }
    else if(isParallel == 0) {
        // vertical parking(deprecated)
    }
    setHandle(MID_ANGLE);
    stop();
}
