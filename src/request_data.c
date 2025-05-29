#include "request_data.h"

HeaderList* makeList(size_t initialLength) {
    Header* data;
    if ((data = calloc(initialLength, sizeof(Header))) 
        == NULL) { return NULL; }

    HeaderList* list;
    if ((list = malloc(sizeof(HeaderList))) == NULL) {
        return NULL;
    } 

    list->capacity = initialLength;
    list->data = data;
    list->length = initialLength;

    return list;
}

HeaderList* push(HeaderList* list, Header* header) {
    if (list->length >= list->capacity) {
        // expand data array
        list->capacity *= 2;
        Header* temp = realloc(
            list->data, 
            sizeof(Header) * list->capacity
        );
        if (temp == NULL) { return NULL; }
        
        list->data = temp;   
    }

    list->data[list->length++] = *header;
    return list;
}

void insertAt(HeaderList* list, Header* item, size_t position) {
    if (position < list->length) {
        list->data[position] = *item;
    }
} 

Header* getAt(HeaderList* list, size_t position) {
    if (position < list->length) {
        return &list->data[position];
    }
    return NULL;
}

void freeList(HeaderList* list) {
    free(list->data);
    free(list);
}