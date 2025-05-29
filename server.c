#include <stdio.h>
#include <stdlib.h>
#include "utility.h"


int main(int argc, char** argv) {

    if (argc < 3) {
        printf("Usage: mp_server <queue_len> <port> <data_directory>\n");
        exit(0);
    }

    const int QUEUE_LEN = atoi(argv[1]);
    const char* PORT = argv[2];
    const char* DATA_DIR_PATH = argv[3];

    int len = strlen(DATA_DIR_PATH);
    if (DATA_DIR_PATH[len - 1] == '/') {
        DATA_DIR_PATH[len - 1] == '\0';
    }
    
    int sockfd = initSocketAndPort(PORT);

    mainLoop(sockfd, QUEUE_LEN, DATA_DIR_PATH);
    
    return 0;
}






