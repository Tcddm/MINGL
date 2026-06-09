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
                              mgl_linear_align_t cross_align,
                              mgl_linear_align_t main_align,
                              mgl_coord_t start_x,mgl_coord_t start_y,
                              mgl_coord_t cross_size,
                              mgl_coord_t main_size){
    mgl_coord_t total_main=0;
    for(uint8_t i=0;i<count;i++){
        if(direction==MGL_LINEAR_HORIZONTAL){
            total_main=(mgl_coord_t)(total_main+items[i].width);
        }else{
            total_main=(mgl_coord_t)(total_main+items[i].height);
        }
    }

    if(count>1){total_main=(mgl_coord_t)(total_main+(count-1)*spacing);}

    mgl_coord_t remaining=(mgl_coord_t)(main_size-total_main);

    mgl_coord_t cursor=0;
    if(remaining>0){
        if(main_align==MGL_ALIGN_CENTER){
            cursor=(mgl_coord_t)(remaining/2);
        }else if(main_align==MGL_ALIGN_END) {
            cursor=remaining;
        }
    }

    for(uint8_t i=0;i<count;i++){
        if(direction==MGL_LINEAR_HORIZONTAL){
            mgl_coord_t offset=mgl_layout_align_offset(cross_align,cross_size,items[i].height);
            items[i].bounds.x=(mgl_coord_t)(start_x+cursor);
            items[i].bounds.y=(mgl_coord_t)(start_y+offset);
            items[i].bounds.w=items[i].width;
            items[i].bounds.h=items[i].height;
            cursor=(mgl_coord_t)(cursor+items[i].width+spacing);
        }else{
            mgl_coord_t offset=mgl_layout_align_offset(cross_align,cross_size,items[i].width);
            items[i].bounds.x=(mgl_coord_t)(start_x+offset);
            items[i].bounds.y=(mgl_coord_t)(start_y+cursor);
            items[i].bounds.w=items[i].width;
            items[i].bounds.h=items[i].height;
            cursor=(mgl_coord_t)(cursor+items[i].height+spacing);
        }
    }
}