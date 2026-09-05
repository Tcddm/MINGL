#include "mgl_timer.h"
#include "logger/mgl_log.h"
#if MGL_TIMER_POOL_SIZE
static mgl_timer_t g_timer_pool[MGL_TIMER_POOL_SIZE];
static uint8_t g_timer_used[MGL_TIMER_POOL_SIZE];
#endif
#if MGL_TIMER_MAX_ACTIVE
static mgl_timer_t *g_active[MGL_TIMER_MAX_ACTIVE];
static uint16_t g_active_count;
#endif

void mgl_timer_init(mgl_timer_t *t,uint32_t ms,bool repeat,
                    void (*cb)(void *data),void *data){
    t->period_ms=ms;
    t->repeat=repeat;
    t->callback=cb;
    t->data=data;
    t->active=0;
    t->start_ms=0;
}

#if MGL_TIMER_MAX_ACTIVE
bool mgl_timer_start(mgl_timer_t *t){
    if(t->active){ return true;}
    uint32_t now=mgl_hal_get_tick_ms();
    for(uint16_t i=0;i<g_active_count;i++){
        if(g_active[i]==t){
            t->start_ms=now;
            t->active=1;
            return true;
        }
    }
    for(uint16_t i=0;i<g_active_count;i++){
        if(!g_active[i]->active){
            g_active[i]=t;
            t->start_ms=now;
            t->active=1;
            return true;
        }
    }
    if(g_active_count>=MGL_TIMER_MAX_ACTIVE){
        MGL_LOG_ERROR(MGL_LOG_TAG_TIMER,"start timer failed: active timer overflow");
        return false;
    }
    t->start_ms=now;
    t->active=1;
    g_active[g_active_count++]=t;
    return true;
}

bool mgl_timer_restart(mgl_timer_t *t){
    if(t->active){
        t->start_ms=mgl_hal_get_tick_ms();
        return true;
    }
    return mgl_timer_start(t);
}

void mgl_timer_stop(mgl_timer_t *t){
    t->active=0;
}

void mgl_timer_tick(void){
    uint32_t now=mgl_hal_get_tick_ms();
    for(uint16_t i=0;i<g_active_count;i++){
        mgl_timer_t *t=g_active[i];
        if(!t->active){ continue;}
        if(now-t->start_ms>=t->period_ms){
            t->callback(t->data);
            if(!t->active){ continue;}
            if(t->repeat){
                t->start_ms=now;
            }else{
                t->active=0;
            }
        }
    }
    uint16_t dst=0;
    for(uint16_t i=0;i<g_active_count;i++){
        if(g_active[i]->active){
            g_active[dst++]=g_active[i];
        }
    }
    g_active_count=dst;
}
#endif

#if MGL_TIMER_POOL_SIZE
mgl_timer_t *mgl_timer_create(uint32_t ms,bool repeat,
                              void (*cb)(void *data),void *data){
    for(uint16_t i=0;i<MGL_TIMER_POOL_SIZE;i++){
        if(g_timer_used[i]){continue;}
        mgl_timer_t *t=&g_timer_pool[i];
        mgl_timer_init(t,ms,repeat,cb,data);
        if(!mgl_timer_start(t)){
            return NULL;
        }
        g_timer_used[i]=true;
        return t;
    }
    MGL_LOG_ERROR(MGL_LOG_TAG_TIMER,"create timer failed: timer pool overflow");
    return NULL;
}

void mgl_timer_destroy(mgl_timer_t *t){
    if(!t){return;}
    mgl_timer_stop(t);
    for(uint16_t i=0;i<MGL_TIMER_POOL_SIZE;i++){
        if(&g_timer_pool[i]==t){
            g_timer_used[i]=false;
            return;
        }
    }
}
#endif