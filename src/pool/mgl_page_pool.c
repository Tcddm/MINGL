#include <string.h>
#include "mgl_page_pool.h"
#include "logger/mgl_log.h"

static uint8_t page_pool[MGL_PAGE_POOL_SIZE];
static uint8_t *pool_top=page_pool;

#define ALIGN4(x)  (((x)+3)&~3)
// #region mgl_page_pool_alloc
void *mgl_page_pool_alloc(uint32_t size){
    uint32_t final_size= ALIGN4(size);
    uint8_t *result = pool_top;
    if (result+final_size>page_pool+MGL_PAGE_POOL_SIZE){
        MGL_LOG_ERROR(MGL_LOG_TAG_PAGE_POOL,
                      "alloc failed: request=%u, aligned=%u, free=%u",
                      size,final_size,
                      (uint32_t)(page_pool+MGL_PAGE_POOL_SIZE-pool_top));
        return NULL;
    }
    pool_top+=final_size;
    MGL_LOG_DBG(MGL_LOG_TAG_PAGE_POOL,
                "alloc %u bytes (req=%u) at %u, free=%u",
                final_size,size,
                (uint32_t)(result-page_pool),
                (uint32_t)(page_pool+MGL_PAGE_POOL_SIZE-pool_top));
    return result;
}
// #endregion

// #region mgl_page_pool_free
void mgl_page_pool_free(void *ptr){
    uint8_t *p=(uint8_t *)ptr;
    if(p>=page_pool&&p<pool_top){
        MGL_LOG_DBG(MGL_LOG_TAG_PAGE_POOL,
                    "free to offset %u, released %u bytes",
                    (uint32_t)(p-page_pool),
                    (uint32_t)(pool_top-p));
        memset(p,0,pool_top-p);
        pool_top=p;
    }else{
        MGL_LOG_ERROR(MGL_LOG_TAG_PAGE_POOL,
                      "free invalid ptr %p (pool: %p..%p)",
                      ptr,page_pool,pool_top);
    }
}
// #endregion

// #region mgl_page_pool_reset
void mgl_page_pool_reset(void){
    MGL_LOG_INFO(MGL_LOG_TAG_PAGE_POOL,"reset (was %u bytes used)",
                 (uint32_t)(pool_top-page_pool));
    memset(page_pool,0,pool_top-page_pool);
    pool_top=page_pool;
}
// #endregion
uint8_t *mgl_page_pool_get_top(void){
    return pool_top;
}