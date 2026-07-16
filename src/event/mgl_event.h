#ifndef MGL_EVENT_H
#define MGL_EVENT_H

#include "widget/mgl_widget.h"

#ifdef __cplusplus
extern "C"{
#endif

// #region MGL_HANDLE_ACTION
/**
* @brief 定义处理所有动作类型的回调函数
*
* 自动提取控件真实类型，用户只需编写动作处理逻辑。
*
* @param name         函数名
* @param widget_type  控件类型（如 mgl_button_t）
* @param func         动作处理代码块，可访问 action 和 widget 变量
*/
#define MGL_HANDLE_ACTION(name,widget_type,func) \
    static bool name(mgl_widget_t * self,const mgl_action_type_t action){ \
        widget_type *widget=container_of(self,widget_type,base); \
                func \
        }
// #endregion

// #region MGL_HANDLE_SINGLE_ACTION
/**
 * @brief 定义处理单一动作类型的回调函数
 *
 * 在自动提取控件真实类型的基础上自动过滤指定动作类型，用户直接编写业务逻辑。
 *
 * @param name         函数名
 * @param widget_type  控件类型（如 mgl_button_t）
 * @param action_name  动作类型名
 * @param func         动作处理代码块，可访问action和widget变量
 */
#define MGL_HANDLE_SINGLE_ACTION(name,widget_type,action_name,func) \
    static bool name(mgl_widget_t * self,const mgl_action_type_t action){ \
        widget_type *widget=container_of(self,widget_type,base); \
        if(action==action_name){ \
            func \
            return true; \
        } \
        return false; \
    }
// #endregion

bool mgl_dispatch_touch_event(mgl_widget_t *root,mgl_event_t *event);
void mgl_process_touch_data(const mgl_touch_data_t *data,mgl_widget_t *root);
mgl_action_type_t mgl_event_default_get_action(mgl_widget_t *self,const mgl_event_t *event);


#ifdef __cplusplus
}
#endif
#endif //MGL_EVENT_H
