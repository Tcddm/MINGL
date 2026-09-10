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
 * 自动提取控件真实类型，用户在下一行编写动作处理逻辑。\n
 * 可访问action和widget变量\n
 * 需要与MGL_HANDLE_ACTION_END()闭合使用
 *
 * @param name         函数名
 * @param widget_type  控件类型（如 mgl_button_t）
*/
#define MGL_HANDLE_ACTION_BEGIN(name,widget_type) \
    static bool name(mgl_widget_t *self,const mgl_action_type_t action){ \
        widget_type *widget=container_of(self,widget_type,base); \
        (void)action;

#define MGL_HANDLE_ACTION_END() }
// #endregion

// #region MGL_HANDLE_SINGLE_ACTION
/**
 * @brief 定义处理单一动作类型的回调函数
 *
 * 在自动提取控件真实类型的基础上自动过滤指定动作类型，用户在下一行编写业务逻辑。\n
 * 可访问action和widget变量\n
 * 需要与MGL_HANDLE_SINGLE_ACTION_END()闭合使用
 *
 * @param name         函数名
 * @param widget_type  控件类型（如 mgl_button_t）
 * @param action_name  动作类型名
 */
#define MGL_HANDLE_SINGLE_ACTION_BEGIN(name,widget_type,action_name) \
    static bool name(mgl_widget_t *self,const mgl_action_type_t action){ \
        widget_type *widget=container_of(self,widget_type,base); \
        if(action==action_name){

#define MGL_HANDLE_SINGLE_ACTION_END() \
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
