#include "overlay/mgl_overlay.h"
#include "timer/mgl_timer.h"
#include "widget/label/mgl_label.h"
#include "page/mgl_page_manager.h"

static mgl_timer_t g_timer;

static void on_toast_timeout(void *data){
    mgl_widget_t *w=(mgl_widget_t *)data;
    mgl_overlay_hide(w);
    mgl_timer_stop(&g_timer);
}

void mgl_toast_show(const char *str,uint32_t duration_ms){
    mgl_widget_t *w=mgl_overlay_find_widget_by_id(MGL_OVERLAY_TOAST);
    if(!w){ return;}
    mgl_label_t *l=container_of(mgl_widget_find_by_id(w,1),mgl_label_t,base);
    if(!l){ return;}
    mgl_label_set_text(l,str);
    mgl_overlay_show(w);
    mgl_timer_init(&g_timer,duration_ms,false,on_toast_timeout,w);
    mgl_timer_restart(&g_timer);
}