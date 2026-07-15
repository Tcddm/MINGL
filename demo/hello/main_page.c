#include "page/mgl_page_manager.h"

#include "painter/mgl_solid_painter.h"

#include "widget/layout/mgl_linear_layout.h"
#include "widget/label/mgl_label.h"

MGL_PAGE_WIDGETS_START(main_page)
MGL_ROOT(
        MGL_LINEAR_LAYOUT(
                .pref_w=g_mgl_screen_width,
                .pref_h=g_mgl_screen_height,
                .main_align=MGL_ALIGN_CENTER,
                .cross_align=MGL_ALIGN_CENTER,
                .painter= MGL_SOLID_PAINTER_TEMP(MGL_COLOR_BLUE),
                .children=MGL_CHILDREN(
                        MGL_LABEL(
                            .painter=MGL_SOLID_PAINTER_TEMP(MGL_COLOR_BLACK),
                            .text=MGL_STR_SIZE("Welcome to MINGL!",24)
                        )
                )

        )
)
MGL_PAGE_WIDGETS_END()
MGL_PAGE_REGISTRY_START()
    MGL_PAGE_ENTRY("main",main_page),
MGL_PAGE_REGISTRY_END()
