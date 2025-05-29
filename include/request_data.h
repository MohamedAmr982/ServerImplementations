#ifndef REQUEST_DATA_H
#define REQUEST_DATA_H

#include <stdlib.h>

typedef struct Header {
    char* name;
    char* value;
} Header;

typedef struct HeaderList {
    int length;
    int capacity;
    Header* data;
} HeaderList;

typedef struct RequestData {
    char* method;
    char* fileName;
    char* httpVersion;
    HeaderList* headers;
} RequestData;


typedef enum HttpCode {
    OK = 200,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    INTERNAL_SERVER_ERROR = 500,
} HttpCode;

HeaderList* makeList(size_t initialLength);

HeaderList* push(HeaderList* list, Header* item);

void insertAt(HeaderList* list, Header* item, size_t position);

Header* getAt(HeaderList* list, size_t position);

void freeList(HeaderList* list);

#endif