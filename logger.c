#include "logger.h"

void LogError(const char* s){
    fprintf(stderr, "[ ERROR ] %s\n", s);
    exit(-1);
}

void LogSuccess(const char* s){
    fprintf(stderr, "[SUCCESS] %s\n", s);
}

void LogWarning(const char* s){
    fprintf(stderr, "[WARNING] %s\n", s);
}
