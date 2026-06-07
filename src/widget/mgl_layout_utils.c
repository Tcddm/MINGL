#include "mgl_layout_utils.h"

void mgl_layout_linear_measure(const mgl_layout_item_t *items,uint8_t count,
                               mgl_linear_direction_t direction,uint8_t spacing,
                               mgl_coord_t *out_w,mgl_coord_t *out_h){
    mgl_coord_t main_size=0;
    mgl_coord_t cross_size=0;

    if (count==0){
        *out_w=0;
        *out_h=0;
        return;
    }

    for(uint8_t i=0;i<count;i++){
        if(direction==MGL_LINEAR_HORIZONTAL){
            main_size=(mgl_coord_t)(main_size+items[i].width);
            if(items[i].height>cross_size){cross_size=items[i].height;}
        }else{
            main_size=(mgl_coord_t)(main_size+items[i].height);
            if(items[i].width>cross_size){cross_size=items[i].width;}
        }
    }

    if(count>1){main_size=(mgl_coord_t)(main_size+(count-1)*spacing);}

    if(direction==MGL_LINEAR_HORIZONTAL){
        *out_w=main_size;
        *out_h=cross_size;
    }else{
        *out_w=cross_size;
        *out_h=main_size;
    }
}

void mgl_layout_linear_layout(mgl_layout_item_t *items,uint8_t count,
                              mgl_linear_direction_t direction,uint8_t spacing,
                              mgl_linear_align_t align,
                              mgl_coord_t start_x,mgl_coord_t start_y,
                              mgl_coord_t cross_size){
    mgl_coord_t cursor=0;

    for(uint8_t i=0;i<count;i++){
        if(direction==MGL_LINEAR_HORIZONTAL){
            mgl_coord_t offset=mgl_layout_align_offset(align,cross_size,items[i].height);
            items[i].bounds.x=(mgl_coord_t)(start_x+cursor);
            items[i].bounds.y=(mgl_coord_t)(start_y+offset);
            items[i].bounds.w=items[i].width;
            items[i].bounds.h=items[i].height;
            cursor=(mgl_coord_t)(cursor+items[i].width+spacing);
        }else{
            mgl_coord_t offset=mgl_layout_align_offset(align,cross_size,items[i].width);
            items[i].bounds.x=(mgl_coord_t)(start_x+offset);
            items[i].bounds.y=(mgl_coord_t)(start_y+cursor);
            items[i].bounds.w=items[i].width;
            items[i].bounds.h=items[i].height;
            cursor=(mgl_coord_t)(cursor+items[i].height+spacing);
        }
    }
}