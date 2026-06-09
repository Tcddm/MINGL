#ifndef MGL_LAYOUT_UTILS_H
#define MGL_LAYOUT_UTILS_H

#include "core/mgl_types.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef enum{
    MGL_LINEAR_HORIZONTAL,
    MGL_LINEAR_VERTICAL
} mgl_linear_direction_t;

typedef enum {
    MGL_ALIGN_START=0,
    MGL_ALIGN_CENTER=1,
    MGL_ALIGN_END=2,
} mgl_linear_align_t;

typedef struct {
    mgl_coord_t width;
    mgl_coord_t height;
    mgl_rect_t  bounds;
} mgl_layout_item_t;


void mgl_layout_linear_measure(const mgl_layout_item_t *items,uint8_t count,
                               mgl_linear_direction_t direction,uint8_t spacing,
                               mgl_coord_t *out_w,mgl_coord_t *out_h);


void mgl_layout_linear_layout(mgl_layout_item_t *items,uint8_t count,
                              mgl_linear_direction_t direction,uint8_t spacing,
                              mgl_linear_align_t cross_align,
                              mgl_linear_align_t main_align,
                              mgl_coord_t start_x,mgl_coord_t start_y,
                              mgl_coord_t cross_size,
                              mgl_coord_t main_size);


static inline mgl_coord_t mgl_layout_align_offset(mgl_linear_align_t align,
                                                  mgl_coord_t cross_size,
                                                  mgl_coord_t item_cross_size){
    if(align==MGL_ALIGN_CENTER){return (mgl_coord_t)((cross_size-item_cross_size)/2);}
    if(align==MGL_ALIGN_END){return (mgl_coord_t)(cross_size-item_cross_size);}
    return 0;
}

static inline mgl_coord_t mgl_layout_align_offset_with_margin(mgl_linear_align_t align,
                                                              mgl_coord_t cross_size,
                                                              mgl_coord_t child_content,
                                                              mgl_coord_t child_margin_start,
                                                              mgl_coord_t child_margin_end){
    mgl_coord_t total=(mgl_coord_t)(child_margin_start+child_content+child_margin_end);
    mgl_coord_t free=(mgl_coord_t)(cross_size-total);
    mgl_coord_t base=0;
    if(align==MGL_ALIGN_CENTER){
        base=(mgl_coord_t)(free/2);
    }else if(align==MGL_ALIGN_END){
        base=free;
    }
    return (mgl_coord_t)(base+child_margin_start);
}


#ifdef __cplusplus
}
#endif
#endif //MGL_LAYOUT_UTILS_H
