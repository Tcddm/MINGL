cmake_minimum_required(VERSION 3.27)
project(MINGL C)

set(CMAKE_C_STANDARD 11)

set(MGL_CORE_SOURCES
        src/draw/mgl_draw_ctx.c
        src/draw/mgl_draw_ctx_arc.c
        src/draw/mgl_draw_ctx_circle.c
        src/draw/mgl_draw_ctx_fill_polygon.c
        src/draw/mgl_draw_ctx_round_rect.c
        src/draw/mgl_draw_ctx_bitmap.c
        src/draw/mgl_draw_ctx_text.c
        src/core/mgl_core.c
        src/core/mgl_math.c
        src/pool/mgl_page_pool.c
        src/widget/mgl_widget.c
        src/widget/mgl_widget_blueprint.c
        src/widget/layout/mgl_linear_layout.c
        src/widget/label/mgl_label.c
        src/widget/button/mgl_button.c
        src/widget/mgl_layout_utils.c
        src/page/mgl_page.c
        src/page/mgl_render.c
        src/page/mgl_page_manager.c
        src/logger/mgl_log.c
        src/event/mgl_event.c
        src/hal/mgl_hal_sw.c
        tools/mgl_font.c
        src/core/mgl_rect.c
        src/painter/mgl_empty_painter.c
        src/widget/slider/mgl_slider.c
)

add_library(mgl_core STATIC ${MGL_CORE_SOURCES})
target_include_directories(mgl_core PUBLIC ${CMAKE_SOURCE_DIR}/src)


if(NOT MGL_PLATFORM)
    set(MGL_PLATFORM "sdl")
endif()

if(MGL_PLATFORM STREQUAL "sdl")
    target_compile_definitions(mgl_core PRIVATE USE_SDL_CONFIG)
endif()


add_subdirectory(platform/${MGL_PLATFORM})


set(MGL_APP_SOURCES
        platform/${MGL_PLATFORM}/main_${MGL_PLATFORM}.c
)

add_executable(${PROJECT_NAME} ${MGL_APP_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE mgl_core mgl_platform)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:DEBUG=1>
)