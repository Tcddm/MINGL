cmake_minimum_required(VERSION 3.27)
project(MINGL C)

set(CMAKE_C_STANDARD 11)

option(MGL_DEV_DEMO "Build dev demo pages" ON)

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
        src/core/mgl_rect.c
        src/painter/mgl_empty_painter.c
        src/widget/slider/mgl_slider.c
        src/widget/scrollbar/mgl_scrollbar.c
        src/widget/list/mgl_list.c
)

file(GLOB MGL_FONT_SOURCES generated/font/mgl_*_font_*.c)
list(APPEND MGL_CORE_SOURCES ${MGL_FONT_SOURCES})

add_library(mgl_core STATIC ${MGL_CORE_SOURCES})
target_include_directories(mgl_core PUBLIC ${CMAKE_SOURCE_DIR}/src)

if(NOT MGL_PLATFORM)
    set(MGL_PLATFORM "sdl")
endif()

if(MGL_PLATFORM STREQUAL "sdl" AND EXISTS "${CMAKE_SOURCE_DIR}/platform/sdl/mgl_config_sdl.h")
    target_compile_definitions(mgl_core PUBLIC USE_SDL_CONFIG)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/platform/${MGL_PLATFORM}/CMakeLists.txt")
    add_subdirectory(platform/${MGL_PLATFORM})
else()
    add_library(mgl_platform INTERFACE)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/platform/${MGL_PLATFORM}/main_${MGL_PLATFORM}.c")
    set(MGL_APP_SOURCES
            platform/${MGL_PLATFORM}/main_${MGL_PLATFORM}.c
    )

    if(MGL_DEV_DEMO)
        foreach(f IN ITEMS
                demo/dev/main_page.c
                demo/dev/count/count_page.c
                demo/dev/list_test/list_page.c
        )
            if(EXISTS "${CMAKE_SOURCE_DIR}/${f}")
                list(APPEND MGL_APP_SOURCES ${f})
            endif()
        endforeach()
    endif()

    add_executable(${PROJECT_NAME} ${MGL_APP_SOURCES})
    target_link_libraries(${PROJECT_NAME} PRIVATE mgl_core mgl_platform)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:DEBUG=1>
    )
endif()