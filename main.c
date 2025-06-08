#include <stdio.h>
#include <stdlib.h>
#include "server.h"


int main(int argc, char** argv) {

    if (argc < 4) {
        printf("Usage: server <queue_len> <port> <data_directory> <mode>\n");
        printf("mode = 0: single threaded server.\n");
        printf("mode = 1: 1 process/port.\n");
        printf("mode = 2: 1 thread/request.\n");
        exit(0);
    }

    const int QUEUE_LEN = atoi(argv[1]);
    const char* PORT = argv[2];
    
    // directory path checks
    if (!directoryExists(argv[3])) {
        perror("Directory does not exist.\n");
        exit(1);
    }
    // removing '/' at the end of path if exists
    int len = strlen(argv[3]);
    char dataDirPath[len + 1];
    strcpy(dataDirPath, argv[3]);

    if (dataDirPath[len - 1] == '/') {
        dataDirPath[len - 1] = '\0';
    }

    ServerMode mode = atoi(argv[4]);
    
    int sockfd = initSocketAndPort(PORT);

    mainLoop(sockfd, QUEUE_LEN, dataDirPath, mode);
    
    return 0;
}
