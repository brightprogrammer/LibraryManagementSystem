/**
 * @file menu.h
 * @date 8th June 2022
 * @copyright Copyright (c) GroupOne Software Solutions. All Rights Reserved.
 * */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>

void Logger_PrintError(const char* s){
    fprintf(stderr, "[ ERROR ] %s\n", s);
}

void Logger_PrintSuccess(const char* s){
    fprintf(stderr, "[SUCCESS] %s\n", s);
}

void Logger_PrintWarning(const char* s){
    fprintf(stderr, "[WARNING] %s\n", s);
}

#endif // LOGGER_H_
