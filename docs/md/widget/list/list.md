# 列表（MGL_LIST）

列表通过回收池管理大量行数据，每行可以是一个完整的控件子树。内嵌滚动条，支持触摸滚动。

## 数据源
列表不直接持有数据，而是通过Adapter回调从外部获取：
回调|说明
-|-
`get_count(user_data)`|返回总行数
`get_height(user_data,index)`|返回行高（>0 固定高度，0 自动测量）
`bind(user_data,slot_root,index)`|将数据填入某行的控件树

Adapter在列表首次填充或回收池重新分配slot时被调用。`slot_root`是`item_blueprint`实例化出的控件树根，通过`mgl_widget_find_by_id`更新其中的子控件内容。

## 参数
### 基础参数
<!--@include: @/snippets/widget_base_args.md-->
### 控件独有参数
参数|类型|默认值|说明
-|-|-|-|
adapter|const [mgl_list_adapter_t](/core/types)|NULL|数据源
item_blueprint|const [mgl_widget_blueprint_t](/core/types#mgl_widget_blueprint_t)|NULL|列表项模版
scrollbar_args|[mgl_scrollbar_args_t](/widget/scrollbar/scrollbar#控件独有参数)|—|内嵌滚动条的配置
### 框架便捷宏参数
参数|类型|默认值|说明
-|-|-|-|
painter|[mgl_painter_t](/core/types#mgl_painter_t)|主题背景色|列表背景
<!--@include: @/snippets/widget_round_radius_args.md-->

## API
```c
/**
 * @brief 滚动到指定行
 *
 * @param list 列表
 * @param index 索引
 */
void mgl_list_scroll_to(mgl_list_t *list,uint16_t index);
/**
 * @brief 获取当前滚动位置
 *
 * @param list 列表
 * @return 当前滚动位置
 */
int32_t mgl_list_get_scroll_position(const mgl_list_t *list);
/**
 * @brief 获取某行的控件树根（可直接修改内容）
 *
 * @param list 列表
 * @param index 索引
 * @return 树根控件指针
 */
mgl_widget_t *mgl_list_get_slot_root(const mgl_list_t *list,
                                     uint16_t index);
/**
 * @brief 某行高度变化时通知列表重建布局
 * @param list 列表
 * @param index 索引
 * @param new_h 新高度
 */
void mgl_list_notify_height_changed(mgl_list_t *list,uint16_t index,
                                    int16_t new_h);
```

## 动作

列表的触摸事件由内部处理，不通过action_handler回调。

## 回收机制

列表在首次布局时根据实际视口高度计算最优槽位数（可见行数+1）。蓝图实例按需创建，零浪费。滚动时，离开屏幕的slot被回收并绑定到新出现的数据行。`item_blueprint`在每个slot创建时实例化一次，之后通过`bind`回调反复填入不同行的数据。

页面池大小需满足slot实例的总内存需求。含嵌套布局的item建议页面池容量 ≥ 8192。

## 示例

```c
//文本缓冲区，避免文本生命周期问题
static char text_buf[500][32]; 
//共有多少项
static uint16_t my_get_count(void *data) { return 500; }

//每项高度
static mgl_coord_t my_get_height(void *data,uint16_t i) {
    return (i < 5) ? 70 : 50;
}

//数据绑定
static void my_bind(void *data,mgl_widget_t *slot,uint16_t i) {
    mgl_widget_t *label=mgl_widget_find_by_id(slot,1);
    sprintf(text_buf[i],"Item #%d",i);
    mgl_label_set_text(container_of(label,mgl_label_t,base),text_buf[i]);
}

//定义数据源
static const mgl_list_adapter_t kAdapter={
    .get_count=my_get_count,
    .get_height=my_get_height,
    .bind=my_bind
};

//定义列表项模板
MGL_COMPONENT(kItem,
    MGL_LINEAR_LAYOUT(
        .direction=MGL_LINEAR_HORIZONTAL,
        .children=MGL_CHILDREN(
            MGL_LABEL(
                .id=1,
                .font=&mgl_font_16px
            )
        )
    )
);

//声明列表
MGL_LIST(
    .adapter=&kAdapter,
    .item_blueprint=kItem,
    .pref_w=-2,
    .pref_h=-2,
    .scrollbar_args={
        .track=MGL_SOLID_PAINTER_TEMP(MGL_COLOR(48,48,48)),
        .thumb=MGL_SOLID_PAINTER_TEMP(MGL_COLOR(200,200,200))
    }
)