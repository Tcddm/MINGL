#include "mgl_theme.h"
#include "page/mgl_page_manager.h"
const mgl_theme_t g_mgl_theme_light={
        .bg={.type=MGL_PAINTER_TYPE_SOLID,.solid_color=MGL_COLOR(245,245,245)},
        .fg={.type=MGL_PAINTER_TYPE_SOLID,.solid_color=MGL_COLOR(30,30,30)},
        .accent={.type=MGL_PAINTER_TYPE_SOLID,.solid_color=MGL_COLOR(0,120,212)},
        .track={.type=MGL_PAINTER_TYPE_SOLID,.solid_color=MGL_COLOR(180,180,180)},
        .round_radius=6,
};

const mgl_theme_t g_mgl_theme_dark={
        .bg={.type=MGL_PAINTER_TYPE_SOLID,.solid_color=MGL_COLOR(30,30,30)},
        .fg={.type=MGL_PAINTER_TYPE_SOLID,.solid_color=MGL_COLOR(220,220,220)},
        .accent={.type=MGL_PAINTER_TYPE_SOLID,.solid_color=MGL_COLOR(74,144,226)},
        .track={.type=MGL_PAINTER_TYPE_SOLID,.solid_color=MGL_COLOR(80,80,80)},
        .round_radius=6,
};

const mgl_theme_t *g_mgl_theme=&g_mgl_theme_light;

void mgl_theme_set(const mgl_theme_t *theme) {
    g_mgl_theme=theme;
    mgl_current_page_redraw();
}
