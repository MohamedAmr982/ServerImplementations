#include "utility.h"


int initSocketAndPort(const char* port) {
    // prepare addr info for port
    // create a new socket
    // bind socket to port

    int sockfd;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, port, &hints, &res);

    bool bindSuccess = false;

    for (struct addrinfo* curr = res; 
            curr != NULL; curr = curr->ai_next) {
        if ((sockfd = socket(res->ai_family, 
                res->ai_socktype, res->ai_protocol)) == -1) {
            continue;
        }
        if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
            continue;
        }
        bindSuccess = true;
        break;
    }

    if (!bindSuccess) {
        perror("Could not bind to port.\n");
        exit(1);
    }

    freeaddrinfo(res);
    return sockfd;
}

void prepareFilePath(char* path, size_t pathLen, const char* dataDirPath, const char* fileName) {
    memset(path, 0, sizeof(char) * pathLen);
    strcat(path, dataDirPath);
    strcat(path, "/");
    strcat(path, fileName);
}

size_t buildHeader(char* buff, HttpCode code, size_t contentLength) {
    const char* VERSION = "HTTP/1.1";
    char* codeName;
    switch (code) {
        case OK:
            codeName = "OK";
            break;
        case NOT_FOUND:
            codeName = "NOT FOUND";
            break;
        case METHOD_NOT_ALLOWED:
            codeName = "METHOD NOT ALLOWED";
            break;
        case INTERNAL_SERVER_ERROR:
            codeName = "INTERNAL SERVER ERROR";
            break;
    }
    
    size_t len;
    
    const char* errorMsg = "<html><h1>Error</h1></html>";

    if (contentLength == -1) {
        len = sprintf(
            buff,
            "%s %d %s\r\n%s: %s\r\n%s: %d\r\n\r\n%s", 
            "HTTP/1.1", 
            (int) code, 
            codeName,
            "Content-Type",
            "text/html; charset=UTF-8",
            "Content-Length",
            (int) strlen(errorMsg),
            errorMsg
        );
    } else {
        len = sprintf(
            buff,
            "%s %d %s\r\n%s: %s\r\n%s: %d\r\n\r\n", 
            "HTTP/1.1", 
            (int) code, 
            codeName,
            "Content-Type",
            "text/html; charset=UTF-8",
            "Content-Length",
            (int) contentLength
        );
    }
    

    return len;
}

void sendResponse(int sockfd, const char* buff, size_t size) {
    int totalSent = 0;
    while (totalSent < size) {
        int bytesSent = send(sockfd, buff + totalSent, size, 0);
        if (bytesSent == -1) { 
            printf("errno: %s\n", strerror(errno));
            return;
        }
        totalSent += bytesSent;
    }
}

void sendFile(int sockfd, int filefd, size_t size) {
    off_t offset = 0;
    size_t totalSent = 0;
    while (totalSent < size) {
        ssize_t bytesSent = sendfile(
            sockfd, filefd, &offset, size - totalSent);
        if (bytesSent == -1) { 
            printf("errno: %s\n", strerror(errno));
            return; 
        }
        totalSent += bytesSent;
    }
}

int handleRequest(int commSockfd, const char* dataDirPath) {
    
    char readBuff[BUFF_SIZE + 1];
    char writeBuff[BUFF_SIZE + 1];
    int bytesRead;

    if ((bytesRead = 
            recv(commSockfd, readBuff, BUFF_SIZE, 0)) == -1) {
        // return 500 (could not read request)
        size_t len = buildHeader(writeBuff, INTERNAL_SERVER_ERROR, -1);
        sendResponse(commSockfd, writeBuff, len);
        return -1;
    }

    if (bytesRead == 0) {
        // connection was closed by client
        return 0;
    }

    readBuff[bytesRead] = '\0';

    RequestData request;
    parseRequest(readBuff, &request);


    if (strcmp(request.method, "GET")) {
        // return 405 
        size_t len = buildHeader(writeBuff, METHOD_NOT_ALLOWED, -1);
        sendResponse(commSockfd, writeBuff, len);
        freeList(request.headers);
        return -1;
    }

    // check if file exists

    // len data dir path + len '/' + len file name + len terminator
    size_t n = strlen(dataDirPath) + 1 + strlen(request.fileName) + 1;
    char filePath[n];
    prepareFilePath(filePath, n, dataDirPath, request.fileName);

    int filefd = open(filePath, O_RDONLY);

    printf("requested file: %s\n", filePath);

    // incorrect path, executable is inside build dir, does not contain data dir

    if (errno == 2) {
        // 404
        size_t len = buildHeader(writeBuff, NOT_FOUND, -1);
        sendResponse(commSockfd, writeBuff, len);
        freeList(request.headers);
        close(filefd);
        return -1;
    } else if (filefd == -1) {
        // 500
        size_t len = buildHeader(writeBuff, INTERNAL_SERVER_ERROR, -1);
        sendResponse(commSockfd, writeBuff, len);
        freeList(request.headers);
        close(filefd);
        return -1;
    }
    

    struct stat fileStatus;
    fstat(filefd, &fileStatus);
    off_t fileSize = fileStatus.st_size;
    
    // 200 ok, send file
    size_t len = buildHeader(writeBuff, OK, fileSize);
    sendResponse(commSockfd, writeBuff, len);
    sendFile(commSockfd, filefd, fileSize);

    close(filefd);
    freeList(request.headers);
    return 1;
}

int parseRequest(char* buff, RequestData* requestData) {

    // NOTE: both buff and requestData should exist in the same
    // lifetime because parseRequest sets requestData members to
    // pointers to locations in buff

    if (requestData == NULL) { return -1; }

    // HeaderList* headerList = makeList(10);
    // request->headers = headerList;
    requestData->headers = NULL;

    char* method = strtok(buff, " ");
    requestData->method = method;

    char* fileName = strtok(NULL, " ");
    // make sure that the file name does not start with /
    if (fileName[0] == '/') { fileName++; }
    requestData->fileName = fileName;

    char* httpVersion = strtok(NULL, "\r\n");
    requestData->httpVersion = httpVersion;

    // ignore headers for now

    // char* headerName, *headerValue;
    // // headers
    // while ((headerName = strtok(NULL, ":\n")) != NULL) {
    //     headerValue = strtok(NULL, "\r\n ");
    //     Header header = {
    //         .name = headerName,
    //         .value = headerValue
    //     };
    //     push(headerList, &header);
    // }

    return 0;
}

bool directoryExists(const char* dirPath) {
    struct stat dirStatus;
    return !stat(dirPath, &dirStatus) 
        && S_ISDIR(dirStatus.st_mode);
}
