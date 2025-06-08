#ifndef SERVER_H
#define SERVER_H

#include "mp_util.h"
#include "mt_util.h"
#include "utility.h"

typedef enum ServerMode {
    SINGLE_THREADED, MULTIPROCESS, MULTITHREADED
} ServerMode;

void mainLoop(int sockfd, int queueLen, const char* dataDir, ServerMode mode);

void singleThreadedServer(int sockfd, const char* dataDir);

#endif