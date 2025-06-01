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

    if (list == NULL || header == NULL) { return list; }

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

    if (list == NULL || item == NULL) { return; }

    if (position < list->length) {
        list->data[position] = *item;
    }
} 

Header* getAt(HeaderList* list, size_t position) {

    if (list == NULL || position >= list->length) { return NULL; }

    return &list->data[position];
}

void freeList(HeaderList* list) {
    if (list != NULL) {
        free(list->data);
    }
    free(list);
}