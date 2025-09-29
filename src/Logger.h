
#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <cstdio>

typedef int LEVEL;

#define LEVEL_DEBUG -1
#define LEVEL_INFO 0
#define LEVEL_WARN 1
#define LEVEL_ERROR 2

inline void LOG(LEVEL level, std::string message) {
    switch (level) {
        case LEVEL_DEBUG:
            printf("\x1B[32m[DEBUG]\033[0m ");
            break;
        case LEVEL_INFO:
            printf("[INFO]  ");
            break;
        case LEVEL_WARN:
            printf("\x1B[33m[WARN]\033[0m  ");
            break;
        case LEVEL_ERROR:
            printf("\x1B[31m[ERROR]\033[0m ");
            break;
    }
    
    printf("%s\n", message.c_str());
    fflush(stdout);
}

inline void LOG_DEBUG(std::string message) {
    LOG(LEVEL_DEBUG, message);
}

inline void LOG_INFO(std::string message) {
    LOG(LEVEL_INFO, message);
}

inline void LOG_WARN(std::string message) {
    LOG(LEVEL_WARN, message);
}

inline void LOG_ERROR(std::string message) {
    LOG(LEVEL_ERROR, message);
}