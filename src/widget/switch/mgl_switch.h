#ifndef MGL_SWITCH_H
#define MGL_SWITCH_H

#include "widget/mgl_widget.h"
#include "painter/mgl_painter.h"

#ifdef __cplusplus
extern "C"{
#endif


#ifndef MGL_SWITCH_DEFAULT_TRACK_H
#define MGL_SWITCH_DEFAULT_TRACK_H 25
#endif

void *mgl_switch_init(void *memory,const void *args);

// #region struct
/**
 * 控件参数结构体
 */
typedef struct{
    bool value; ///<默认状态
    uint8_t track_h; ///<轨道高度
    MGL_WIDGET_BASE_ARGS_FIELD ///<基础参数
    MGL_WIDGET_PAINTER_ARGS_FIELD_NAME(track) ///<轨道画笔
    MGL_WIDGET_PAINTER_ARGS_FIELD_NAME(thumb) ///<滑块画笔
} mgl_switch_args_t;

/**
 * 控件结构体
 */
typedef struct{
    mgl_widget_t base; ///<基类，必须放在第一位
    bool value; ///<当前状态
    uint8_t track_h; ///<轨道高度
    MGL_WIDGET_PAINTER_FIELD_NAME(track) ///<轨道画笔（注意没有ARGS）
    MGL_WIDGET_PAINTER_FIELD_NAME(thumb) ///<滑块画笔（注意没有ARGS）
} mgl_switch_t;

// #endregion

#define MGL_SWITCH(...) \
    &((mgl_widget_blueprint_t){ \
        .init=mgl_switch_init, \
        .size=sizeof(mgl_switch_t), \
        .args=&(mgl_switch_args_t ){ __VA_ARGS__ } \
    })

#ifdef __cplusplus
}
#endif
#endif //MGL_SWITCH_H
