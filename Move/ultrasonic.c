//
// Created by Hwiyong on 2019-08-12.
//

#include "ultrasonic.h"

void initUltrasonic(int triggerPin, int echoPin) {
    pinMode(triggerPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

int getUltrasonicDistance(int triggerPin, int echoPin) {
    long startTime, endTime;

    digitalWrite(triggerPin, LOW);
    digitalWrite(triggerPin, HIGH);
    usleep(10);
    digitalWrite(triggerPin, LOW);

    while(digitalRead(echoPin) == LOW);
    startTime = micros();

    while(digitalRead(echoPin) == HIGH);
    endTime = micros();

    return (int)(endTime - startTime) / 58;
}