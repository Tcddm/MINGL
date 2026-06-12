#include "mgl_painter.h"
#include "hal/mgl_hal.h"

static void fill_rect(const mgl_painter_t *self,mgl_coord_t x,mgl_coord_t y,mgl_coord_t w,mgl_coord_t h){

}

const mgl_painter_vtable_t g_mgl_empty_painter_vtable={
        .fill_rect=fill_rect,
        .set_pixel=NULL
};