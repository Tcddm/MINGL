#ifndef MGL_ARRAY_H
#define MGL_ARRAY_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

void *mgl_array_insert(void *base,uint16_t elem_size,
                       uint16_t *count,uint16_t capacity,
                       uint16_t index);

bool mgl_array_remove(void *base,uint16_t elem_size,
                      uint16_t *count,uint16_t index);

#ifdef __cplusplus
}
#endif

#endif //MGL_ARRAY_H
