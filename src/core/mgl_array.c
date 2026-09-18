#include "mgl_array.h"
#include <string.h>

void *mgl_array_insert(void *base,uint16_t elem_size,
                       uint16_t *count,uint16_t capacity,
                       uint16_t index){
    if(!base || !count || elem_size==0){ return NULL;}
    if(index>*count){ return NULL;}
    if(*count>=capacity){ return NULL;}

    uint8_t *p=(uint8_t *)base;
    memmove(p+(index+1)*elem_size,
            p+index*elem_size,
            (size_t)(*count-index)*elem_size);

    (*count)++;
    return p+index*elem_size;
}

bool mgl_array_remove(void *base,uint16_t elem_size,
                      uint16_t *count,uint16_t index){
    if(!base || !count || elem_size==0){ return false;}
    if(index>=*count){ return false;}

    uint8_t *p=(uint8_t *)base;
    memmove(p+index*elem_size,
            p+(index+1)*elem_size,
            (size_t)(*count-index-1)*elem_size);

    (*count)--;
    return true;
}
