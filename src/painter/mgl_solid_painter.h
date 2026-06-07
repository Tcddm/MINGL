#ifndef MGL_SOLID_PAINTER_H
#define MGL_SOLID_PAINTER_H

#include "mgl_painter.h"

#ifdef __cplusplus
extern "C"{
#endif


#define MGL_SOLID_PAINTER_TEMP(col) \
    (&(const mgl_painter_t){ \
        .type=MGL_PAINTER_TYPE_SOLID, \
        .solid_color=col \
    })

#ifdef __cplusplus
}
#endif
#endif //MGL_SOLID_PAINTER_H
