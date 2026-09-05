#ifndef MGL_TIMER_H
#define MGL_TIMER_H

#include "hal/mgl_hal.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct {
    bool active;
    bool repeat;
    uint32_t period_ms;
    uint32_t start_ms;
    void (*callback)(void *data);
    void *data;
} mgl_timer_t;

void mgl_timer_init(mgl_timer_t *t,uint32_t ms,bool repeat,
                     void (*cb)(void *data),void *data);
#if MGL_TIMER_MAX_ACTIVE
bool mgl_timer_start(mgl_timer_t *t);
bool mgl_timer_restart(mgl_timer_t *t);
void mgl_timer_stop(mgl_timer_t *t);
void mgl_timer_tick(void);
#else
static inline bool mgl_timer_start(mgl_timer_t *t){(void)t;return false;}
static inline bool mgl_timer_restart(mgl_timer_t *t){(void)t;return false;}
static inline void mgl_timer_stop(mgl_timer_t *t){(void)t;}
static inline void mgl_timer_tick(void){}
#endif

#if MGL_TIMER_POOL_SIZE
mgl_timer_t *mgl_timer_create(uint32_t ms,bool repeat,
                              void (*cb)(void *data),void *data);
void mgl_timer_destroy(mgl_timer_t *t);
#else
static inline mgl_timer_t *mgl_timer_create(uint32_t ms,bool repeat,
                              void (*cb)(void *data),void *data){
    (void)ms;(void)repeat;(void)cb;(void)data;return NULL;
}
static inline void mgl_timer_destroy(mgl_timer_t *t){(void)t;}
#endif

#ifdef __cplusplus
}
#endif

#endif //MGL_TIMER_H
