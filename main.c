#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

// maximum number of lines terminal can show at once
// 25 is the default terminal height that
#define TERMINAL_HEIGHT 25

/**
 * Print intro text about this software.
 * */
void PrintIntro(){
    puts(";---------------------------------------------------------------------;");
    puts("; LiMS Library Management System");
    puts("; Copyright (c) GroupOne Software Solutions 2022. All Rights Reserved ;");
    puts(";---------------------------------------------------------------------;");
    puts("; [q : Quit]");
    puts(";---------------------------------------------------------------------;");
}

/**
 * Resets the screen by printing several new lines
 * */
void ResetScreen(){
    for(uint8_t i = 0; i < TERMINAL_HEIGHT; i++) puts("");
}

int main(){
    bool is_running = true;

    while(is_running){
        // reset screen and print intro
        ResetScreen();
        PrintIntro();

        // handle input
        char c = getchar();
        if(c == 'q'){
            is_running = false;
        }
    }
}
