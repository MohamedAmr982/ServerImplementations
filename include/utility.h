#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>
#include "request_data.h"

#define BUFF_SIZE 8192


int initSocketAndPort(const char* port);

void mainLoop(int sockfd, int queueLen, const char* dataDir);

void handleRequest(int commSockfd, const char* dataDir);

RequestData* parseRequest(char* buff);

void prepareFilePath(char* path, size_t pathLen, const char* dataDirPath, const char* fileName);

size_t buildHeader(char* buff, HttpCode code, size_t contentLength);

void sendResponse(int sockfd, const char* buff, size_t size);

void sendFile(int sockfd, int filefd, size_t size);

bool directoryExists(const char* dirPath);

#endif