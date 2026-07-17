#ifndef MGL_THEME_H
#define MGL_THEME_H

#include "painter/mgl_painter.h"

#ifdef __cplusplus
extern "C"{
#endif

// #region mgl_theme_t
typedef struct {
    mgl_painter_t bg; ///< 背景色
    mgl_painter_t fg; ///<前景色
    mgl_painter_t accent; ///<强调色
    mgl_painter_t track; ///<轨道色
    mgl_coord_t round_radius; ///<默认圆角
} mgl_theme_t;
// #endregion

extern const mgl_theme_t *g_mgl_theme;
extern const mgl_theme_t g_mgl_theme_light;
extern const mgl_theme_t g_mgl_theme_dark;

void mgl_theme_set(const mgl_theme_t *theme);

static inline const mgl_painter_t *mgl_theme_resolve(uint8_t slot){
    return &((const mgl_painter_t *)g_mgl_theme)[slot];
}

static inline mgl_painter_t mgl_theme_painter_resolve(const mgl_painter_t *p){
    if(p->type == MGL_PAINTER_TYPE_THEMED){
        return *mgl_theme_resolve(p->theme_slot);
    }
    return *p;
}

#ifdef __cplusplus
}
#endif
#endif //MGL_THEME_H
