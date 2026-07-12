#ifndef MGL_WIDGET_BLUEPRINT_H
#define MGL_WIDGET_BLUEPRINT_H

#include "mgl_widget.h"
#include "pool/mgl_page_pool.h"

#ifdef __cplusplus
extern "C"{
#endif

// #region mgl_widget_blueprint_t
/**
 * @brief 控件蓝图结构体
 */
typedef struct {
    void *(*init)(void *memory,const void *args); ///<控件初始化函数
    uint16_t size; ///<控件需要的字节数
    const void *args; ///<传递给初始化函数的参数
} mgl_widget_blueprint_t;
// #endregion

mgl_widget_t *mgl_blueprint_collect(const mgl_widget_blueprint_t *bp,mgl_widget_t *parent);

#ifdef __cplusplus
}
#endif
#endif //MGL_WIDGET_BLUEPRINT_H
