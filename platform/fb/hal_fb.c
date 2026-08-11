#include "hal_fb.h"
#include "mgl_config.h"
#include "hal/mgl_fb.h"
#include "core/mgl_core.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <time.h>
#if MGL_TOUCH_EVDEV
#include <linux/input.h>
#endif

#if MGL_TOUCH_EVDEV
static struct {
    int fd;
    int current_slot;
    int tracking_id[MGL_TOUCH_MAX_FINGERS];
    mgl_coord_t x[MGL_TOUCH_MAX_FINGERS];
    mgl_coord_t y[MGL_TOUCH_MAX_FINGERS];
} g_touch;
#endif

static uint32_t g_start_tick;
static uint16_t *g_fbdev_mmap;
static size_t g_fbdev_size;
static mgl_color_value_t *g_cached_buf;
static uint32_t g_fb_line_length;

bool mgl_hal_fb_init(const char *fb_device){
    int fd=open(fb_device, O_RDWR);
    if(fd<0){
        fprintf(stderr,"Failed to open %s\n",fb_device);
        return false;
    }

    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    if(ioctl(fd,FBIOGET_VSCREENINFO,&vinfo)<0 ||
        ioctl(fd,FBIOGET_FSCREENINFO,&finfo)<0){
        fprintf(stderr,"Failed to get fb info\n");
        close(fd);
        return false;
    }

    uint8_t *fb_mmap=mmap(NULL,finfo.smem_len,
                            PROT_READ | PROT_WRITE,
                            MAP_SHARED,fd,0);
    if(fb_mmap==MAP_FAILED){
        fprintf(stderr,"Failed to mmap framebuffer\n");
        close(fd);
        return false;
    }

    g_fbdev_mmap=(uint16_t *)fb_mmap;
    g_fbdev_size=(size_t)finfo.line_length*vinfo.yres;
    g_fb_line_length=finfo.line_length;

    uint32_t stride_px=(uint32_t)(finfo.line_length/sizeof(uint16_t));
    g_cached_buf=malloc(g_fbdev_size);
    if(!g_cached_buf){
        fprintf(stderr,"Failed to allocate framebuffer\n");
        close(fd);
        return false;
    }
    memset(g_cached_buf,0,g_fbdev_size);

    mgl_fb_t fb={
        .data=g_cached_buf,
        .width=(mgl_coord_t)vinfo.xres,
        .height=(mgl_coord_t)vinfo.yres,
        .stride_px=stride_px
    };
    mgl_hal_set_fb(&fb);

#if MGL_TOUCH_EVDEV
    g_touch.fd=open(MGL_TOUCH_EVDEV_DEVICE,O_RDONLY | O_NONBLOCK);
    for(int i=0;i<MGL_TOUCH_MAX_FINGERS;i++) {
        g_touch.tracking_id[i]=-1;
    }
#endif

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    g_start_tick=(uint32_t)(ts.tv_sec*1000+ts.tv_nsec/1000000);

    return true;
}

uint32_t mgl_hal_get_tick_ms(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    return (uint32_t)(ts.tv_sec*1000+ts.tv_nsec/1000000)-g_start_tick;
}

void mgl_hal_flush_display(mgl_rect_t *flush_rects, uint8_t flush_count){
    if(flush_count==0){
        memcpy(g_fbdev_mmap,g_cached_buf,g_fbdev_size);
        return;
    }
    const uint8_t *src=(const uint8_t *)g_cached_buf;
    uint8_t *dst=(uint8_t *)g_fbdev_mmap;
    for(uint8_t i=0;i<flush_count;i++){
        uint32_t row_bytes=(uint32_t)flush_rects[i].w*sizeof(uint16_t);
        for(mgl_coord_t y=0;y<flush_rects[i].h;y++){
            uint32_t off=(uint32_t)(y+flush_rects[i].y)*g_fb_line_length
                         +(uint32_t)flush_rects[i].x*sizeof(uint16_t);
            memcpy(dst+off,src+off,row_bytes);
        }
    }
}

bool mgl_hal_get_touch(mgl_touch_data_t *touch){
    touch->count=0;
#if MGL_TOUCH_EVDEV
    struct input_event ev;
    while(read(g_touch.fd,&ev,sizeof(ev))==sizeof(ev)){
        if(ev.type==EV_ABS){
            switch(ev.code){
            case ABS_MT_SLOT:
                if(ev.value<MGL_TOUCH_MAX_FINGERS){
                    g_touch.current_slot=ev.value;
                }
                break;
            case ABS_MT_TRACKING_ID:
                if(g_touch.current_slot<MGL_TOUCH_MAX_FINGERS){
                    g_touch.tracking_id[g_touch.current_slot]=ev.value;
                }
                break;
            case ABS_MT_POSITION_X:
                if(g_touch.current_slot<MGL_TOUCH_MAX_FINGERS){
                    g_touch.x[g_touch.current_slot]=(mgl_coord_t)ev.value;
                }
                break;
            case ABS_MT_POSITION_Y:
                if(g_touch.current_slot<MGL_TOUCH_MAX_FINGERS){
                    g_touch.y[g_touch.current_slot]=(mgl_coord_t)ev.value;
                }
                break;
            }
        }
    }
    //将所有已读事件消化完后，用最终追踪状态构建触摸数据
    for(int i=0;i<MGL_TOUCH_MAX_FINGERS;i++){
        if(g_touch.tracking_id[i]>=0) {
            touch->points[touch->count].x=g_touch.x[i];
            touch->points[touch->count].y=g_touch.y[i];
            touch->points[touch->count].pressed=true;
            touch->points[touch->count].finger_id=(uint8_t)i;
            touch->count++;
        }
    }
    return touch->count>0;
#endif
    return false;
}
