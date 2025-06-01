#include <stdio.h>
#include <stdlib.h>
#include "utility.h"


int main(int argc, char** argv) {

    if (argc < 3) {
        printf("Usage: server <queue_len> <port> <data_directory>\n");
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

    
    int sockfd = initSocketAndPort(PORT);

    mainLoop(sockfd, QUEUE_LEN, dataDirPath);
    
    return 0;
}






