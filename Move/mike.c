//
// Created by Hwiyong on 2019-08-29.
//

#include "mike.h"

void record(int recordSecond, char *fileName) {
    char command[100];
    sprintf(command, "arecord -D plughw:1,0 -d %d %s &", recordSecond, fileName);
    system(command);
}

