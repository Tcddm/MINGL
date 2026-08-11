#include "mgl_overlay_default.h"
#include "page/mgl_page_manager.h"
#include "widget/layout/mgl_linear_layout.h"
#include "widget/label/mgl_label.h"
#include "painter/mgl_solid_painter.h"

MGL_PAGE_WIDGETS_START(mgl_make_default_overlay)
MGL_ROOT(
    MGL_LINEAR_LAYOUT(
        .pref_w=g_mgl_screen_width,
        .pref_h=g_mgl_screen_height,
        .main_align=MGL_ALIGN_END,
        .cross_align=MGL_ALIGN_CENTER,
        .direction=MGL_LINEAR_VERTICAL,
        .children=MGL_CHILDREN(
            MGL_LINEAR_LAYOUT(
                .direction=MGL_LINEAR_HORIZONTAL,
                .id=MGL_OVERLAY_TOAST,
                .hidden=1,
                .cross_align=MGL_ALIGN_CENTER,
                .round_radius=10,
                .painter=MGL_SOLID_PAINTER_TEMP(MGL_COLOR(50,50,50)),
                .children=MGL_CHILDREN(
                    MGL_LABEL(
                        .id=1,
                        .text=MGL_STR(""),
                        .painter=MGL_SOLID_PAINTER_TEMP(MGL_COLOR_WHITE),
                        .margin=((mgl_margin_t){8,12,8,12})
                    )
                )
            )
        )
    )
)
MGL_PAGE_WIDGETS_END()
