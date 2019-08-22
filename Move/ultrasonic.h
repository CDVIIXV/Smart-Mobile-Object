//
// Created by Hwiyong on 2019-08-12.
//

#ifndef SMARTMOBILEOBJECT_RASPBERRYPI_ULTRASONIC_H
#define SMARTMOBILEOBJECT_RASPBERRYPI_ULTRASONIC_H

#include <wiringPi.h>
#include <unistd.h>

void initUltrasonic(int triggerPin, int echoPin);
int getUltrasonicDistance(int triggerPin, int echoPin);

#endif //SMARTMOBILEOBJECT_RASPBERRYPI_ULTRASONIC_H
