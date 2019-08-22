//
// Created by Hwiyong on 2019-08-12.
//

#include "key_input.h"

char getch() {
    char c;
    struct termios oldattr, newattr;

    tcgetattr(STDIN_FILENO, &oldattr);	// now terminal config
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);	// turn off CANONICAL and ECHO
    newattr.c_cc[VMIN] = 1;			// least letter count 1
    newattr.c_cc[VTIME] = 0;		// least read wating time 0
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);	// input terminal config
    c = getchar();				// read keyboard
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);	// recover original coonfig
    return c;
}
