//
// Created by Hwiyong on 2019-08-17.
//

#include "audio.h"

void playAudio(char *fileName) {
    char command[100];
    sprintf(command, "omxplayer %s &", fileName);   // background play
    system(command);
}

void stopAudio() {
    system("killall omxplayer.bin");
}