#ifndef MGL_PAGE_POOL_H
#define MGL_PAGE_POOL_H

#include "mgl_config.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"{
#endif

void *mgl_page_pool_alloc(uint32_t size);
void mgl_page_pool_free(void *ptr);
void mgl_page_pool_reset(void);
uint8_t *mgl_page_pool_get_top(void);


#ifdef __cplusplus
}
#endif
#endif //MGL_PAGE_POOL_H
