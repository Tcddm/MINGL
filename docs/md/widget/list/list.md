# 列表（MGL_LIST）

列表通过回收池管理大量行数据，每行可以是一个完整的控件子树。内嵌滚动条，支持触摸滚动。

## 数据源
列表不直接持有数据，而是通过Adapter回调从外部获取：
回调|说明
-|-
`get_count(user_data)`|返回总行数
`get_height(user_data,index)`|返回行高（>0 固定高度，0 自动测量）
`bind(user_data,slot_root,index)`|将数据填入某行的控件树

字段：
类型|名称|默认值|说明
-|-|-|-
void *|user_data|NULL|回调上下文。由调用方设置，列表仅透传给回调。通常指向业务数据结构，用于提供实际数据
uint16_t|capacity|0|容量，为0时不预分配且不可插入，但仍可删除

Adapter在列表首次填充或回收池重新分配slot时被调用。`slot_root`是`item_blueprint`实例化出的控件树根，通过`mgl_widget_find_by_id`更新其中的子控件内容。

## 参数
### 基础参数
<!--@include: @/snippets/widget_base_args.md-->
### 控件独有参数
参数|类型|默认值|说明
-|-|-|-
adapter|const [mgl_list_adapter_t](/core/types)|NULL|数据源
item_blueprint|const [mgl_widget_blueprint_t](/core/types#mgl_widget_blueprint_t)|NULL|列表项模版
scrollbar_args|[mgl_scrollbar_args_t](/widget/scrollbar/scrollbar#控件独有参数)|—|内嵌滚动条的配置
### 框架便捷宏参数
参数|类型|默认值|说明
-|-|-|-
painter|[mgl_painter_t](/core/types#mgl_painter_t)|主题背景画笔|列表背景画笔
<!--@include: @/snippets/widget_round_radius_args.md-->

## API
<<< @/../../src/widget/list/mgl_list.h#api{c}

## 动作

列表的触摸事件由内部处理，不通过action_handler回调。

## 回收机制

列表在首次布局时根据实际视口高度计算最优槽位数（可见行数+1）。蓝图实例按需创建，零浪费。滚动时，离开屏幕的slot被回收并绑定到新出现的数据行。`item_blueprint`在每个slot创建时实例化一次，之后通过`bind`回调反复填入不同行的数据。

页面池大小需满足slot实例的总内存需求。含嵌套布局的item建议页面池容量 ≥ 8192。

## 示例
```c
#ifndef MGL_DEMO_DEV_NEW_LIST_H
#define MGL_DEMO_DEV_NEW_LIST_H

#include "page/mgl_page_manager.h"

MGL_EXPORT_PAGE(make_demo_dev_new_list_page)

#endif //MGL_DEMO_DEV_NEW_LIST_H
```
```c
#include <stdio.h>

#include "widget/layout/mgl_linear_layout.h"
#include "widget/button/mgl_button.h"
#include "widget/list/mgl_list.h"
#include "widget/label/mgl_label.h"

#include "event/mgl_event.h"
#include "painter/mgl_solid_painter.h"
#include "core/mgl_array.h"

//使用宏预定义按钮
#define button(btn_text,action_id) MGL_BUTTON( \
.pref_w=100, \
.pref_h=-1, \
.round_radius=20, \
.text=btn_text, \
.id=action_id, \
.action_handler=on_button_click \
)

//定义数据结构
#define MAX_ITEMS 32
typedef struct {
    char item_buf[32];
    char id_buf[8];
} list_item;
typedef struct {
    list_item items[MAX_ITEMS];
    uint16_t count;
} list_model;

//定义按钮类型
typedef enum {
    BTN_BACK=1,
    BTN_ADD,
    BTN_REMOVE
} btn_type;

//按钮的动作处理回调
MGL_HANDLE_SINGLE_ACTION_BEGIN(on_button_click,mgl_button_t,MGL_ACTION_CLICK)
switch(self->id){
    case BTN_BACK:
        mgl_page_back();
        break;
    case BTN_ADD:{
        mgl_list_t *list=container_of(
                mgl_current_page_find_widget_by_id(100),
                mgl_list_t,base);
        list_model *m=list->adapter.user_data;
        //尾部插入：插入位置=当前数量（即最后一个元素之后）
        uint16_t insert_at=m->count;

        list_item *slot=(list_item *)mgl_array_insert(
                m->items,sizeof(list_item),
                &m->count,MAX_ITEMS,insert_at);
        if(!slot){break;}

        sprintf(slot->item_buf,MGL_FMT("Inserted #%d"),m->count-1);
        sprintf(slot->id_buf,MGL_FMT("#%d"),m->count-1);

        mgl_list_insert(list,insert_at);
    }
        break;
    case BTN_REMOVE: {
        mgl_list_t *list=container_of(
                mgl_current_page_find_widget_by_id(100),mgl_list_t,base);
        //获取这个按钮是在哪个索引的
        uint16_t index=
                mgl_list_get_index_by_widget(list,self);
        if(index==MGL_LIST_INVALID_INDEX){ break;}

        list_model *m=list->adapter.user_data;

        mgl_array_remove(m->items,sizeof(list_item),&m->count,index);
        mgl_list_remove(list,index);
    }
        break;
    default:
        //其他按钮不处理
        return true;
}
MGL_HANDLE_SINGLE_ACTION_END()

//顶部操作按钮控件组
MGL_COMPONENT(btns,
    MGL_LINEAR_LAYOUT(
        .direction=MGL_LINEAR_HORIZONTAL,
        .pref_w=-1,
        .pref_h=50,
        .round_radius=-1,
        .children=MGL_CHILDREN(
            button(MGL_STR("返回"),BTN_BACK),
            button(MGL_STR("新增"),BTN_ADD)
        )
    )
);

//列表项控件组
MGL_COMPONENT(kitem,
    MGL_LINEAR_LAYOUT(
        .direction=MGL_LINEAR_HORIZONTAL,
        .main_align=MGL_ALIGN_SPACE_BETWEEN,
        .cross_align=MGL_ALIGN_CENTER,
        .pref_w=-1,
        .pref_h=-1,
        .round_radius=-1,
        .painter=MGL_SOLID_PAINTER_TEMP(MGL_COLOR_GREEN),
        .children=MGL_CHILDREN(
            MGL_LABEL(
                .id=1,
                .font=&mgl_font_16px,
                .margin=((mgl_margin_t){0,4,0,4})
            ),
            MGL_LABEL(
                .id=2,
                .font=&mgl_font_16px,
                .margin=((mgl_margin_t){0,8,0,8})
            ),
            button(MGL_STR("删除"),BTN_REMOVE)
        )
    )
);

//生成测试数据
static void build_test_data(list_model *m){
    m->count=5;
    for(int i=0;i<m->count;i++) {
        sprintf(m->items[i].item_buf,MGL_FMT("This is item #%d in the list"),i);
        sprintf(m->items[i].id_buf,MGL_FMT("#%d"),i);

    }
}

//数据源：获得数量
static uint16_t get_count(void *user_data){
    return ((list_model *)user_data)->count;
}

//数据源：获得当前索引的列表项的高度
static mgl_coord_t get_height(void *user_data,uint16_t index){
    (void)user_data;
    (void)index;
    //全部高度固定为50
    //0为自动布局，需要确保列表项可以自动测量出高度
    return 50;
}

//数据源：绑定数据
static void bind(void *user_data,mgl_widget_t *slot,uint16_t index){
    list_model *m=(list_model *)user_data;

    mgl_widget_t *w_id=mgl_widget_find_by_id(slot,1);
    mgl_widget_t *w_text=mgl_widget_find_by_id(slot,2);

    if(w_id){
        mgl_widget_set_text(container_of(w_id,mgl_label_t,base),
                            text,m->items[index].id_buf);
    }
    if(w_text){
        mgl_widget_set_text(container_of(w_text,mgl_label_t,base),
                            text,m->items[index].item_buf);
    }
}

//页面声明
MGL_PAGE_WIDGETS_START(make_demo_dev_new_list_page)
    //初始化数据
    mgl_list_adapter_t *adapter=(mgl_list_adapter_t *)mgl_page_pool_alloc(sizeof(mgl_list_adapter_t));
    adapter->user_data=mgl_page_pool_alloc(sizeof(list_model));
    adapter->get_count=get_count;
    adapter->get_height=get_height;
    adapter->bind=bind;
    adapter->capacity=MAX_ITEMS;
    //生成测试数据
    build_test_data(adapter->user_data);
MGL_ROOT(
    MGL_LINEAR_LAYOUT(
        .direction=MGL_LINEAR_VERTICAL,
        .pref_w=g_mgl_screen_width,
        .pref_h=g_mgl_screen_height,
        .round_radius=-1,
        .painter=MGL_THEME_BG(),
        .children=MGL_CHILDREN(
            btns,
            MGL_LIST(
                .id=100,
                .pref_w=-1,
                .pref_h=-1,
                .round_radius=-1,
                .painter=MGL_THEME_BG(),
                .item_blueprint=kitem,
                .adapter=adapter
            )
        )
    )
)
MGL_PAGE_WIDGETS_END()
```