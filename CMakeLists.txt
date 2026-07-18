cmake_minimum_required(VERSION 3.27)
project(MINGL C)

set(CMAKE_C_STANDARD 11)
set(KCONFIG_PY "${CMAKE_SOURCE_DIR}/tools/kconfig.py")
if(WIN32)
    set(_PYTHON_DEFAULT "python")
else()
    set(_PYTHON_DEFAULT "python3")
endif()


set(CONFIG_BOOL_VARS
        MGL_TOOL_ACCEL_FONT_GEN
)

if(WIN32)
    set(Python_EXECUTABLE "${CMAKE_SOURCE_DIR}/tools/.venv/Scripts/python.exe" CACHE FILEPATH "Python interpreter")
else()
    set(Python_EXECUTABLE "${CMAKE_SOURCE_DIR}/tools/.venv/bin/python" CACHE FILEPATH "Python interpreter")
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/tools/.venv")
    message(STATUS "Virtual environment already exists.")
else()
    message(STATUS "Creating virtual environment...")
    execute_process(
            COMMAND ${_PYTHON_DEFAULT} -m venv .venv
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/tools"
            RESULT_VARIABLE _VENV_RES
    )
    if(NOT _VENV_RES EQUAL 0)
        message(FATAL_ERROR "Failed to create virtual environment in tools/.venv")
    endif()

    message(STATUS "Installing dependencies from requirements.txt...")
    if(EXISTS "${CMAKE_SOURCE_DIR}/tools/requirements.txt")
        execute_process(
                COMMAND ${Python_EXECUTABLE} -m pip install -r requirements.txt
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/tools"
                RESULT_VARIABLE _PIP_RES
        )
        if(NOT _PIP_RES EQUAL 0)
            message(FATAL_ERROR "Failed to install Python dependencies")
        endif()
        message(STATUS "Dependencies installed.")
    else()
        message(WARNING "requirements.txt not found, skipping dependency installation")
    endif()
endif()

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/.config")
    execute_process(
            COMMAND ${Python_EXECUTABLE} ${KCONFIG_PY} alldefconfig
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()

set(MGL_CONFIG_HEADER "${CMAKE_SOURCE_DIR}/generated/config/mgl_config_gen.h")

add_custom_command(
        OUTPUT ${MGL_CONFIG_HEADER}
        COMMAND ${Python_EXECUTABLE} ${KCONFIG_PY} genheader
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        DEPENDS ${CMAKE_SOURCE_DIR}/.config
        COMMENT "Generating mgl_config_gen.h from .config"
)

add_custom_target(generate_config DEPENDS ${MGL_CONFIG_HEADER})

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
        src/theme/mgl_theme.c
)

file(STRINGS "${CMAKE_SOURCE_DIR}/.config" CONFIG_LINES)
foreach(line ${CONFIG_LINES})
    if(line MATCHES "^CONFIG_MGL_PLATFORM_([^=]+)=y$")
        string(TOLOWER "${CMAKE_MATCH_1}" MGL_PLATFORM)
    endif()
    if(line MATCHES "^CONFIG_MGL_DEMO_([^=]+)=y$")
        string(TOLOWER "${CMAKE_MATCH_1}" MGL_DEMO)
    endif()
    foreach(var ${CONFIG_BOOL_VARS})
        if(line MATCHES "^CONFIG_${var}=y$")
            set(${var} ON)
        endif()
    endforeach()
endforeach()

if(MGL_TOOL_ACCEL_FONT_GEN)
    file(GLOB MGL_FONT_GENERATED CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/generated/font/mgl_*_font_*.c)
    list(APPEND MGL_CORE_SOURCES ${MGL_FONT_GENERATED})
endif()

add_library(mgl_core STATIC ${MGL_CORE_SOURCES})
add_dependencies(mgl_core generate_config)
target_include_directories(mgl_core PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/generated/config
)

if(MGL_TOOL_ACCEL_FONT_GEN)
    target_include_directories(mgl_core PUBLIC ${CMAKE_SOURCE_DIR}/generated/font)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/platform/${MGL_PLATFORM}/CMakeLists.txt")
    add_subdirectory(platform/${MGL_PLATFORM})
    if(TARGET mgl_platform)
        add_dependencies(mgl_platform generate_config)
        target_include_directories(mgl_platform PUBLIC ${CMAKE_SOURCE_DIR}/generated/config)
    endif()
else()
    add_library(mgl_platform INTERFACE)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/platform/${MGL_PLATFORM}/main_${MGL_PLATFORM}.c")
    set(MGL_APP_SOURCES
            platform/${MGL_PLATFORM}/main_${MGL_PLATFORM}.c
    )

    if(MGL_DEMO)
        set(MGL_FONT_GEN_SCAN_PATH "demo/${MGL_DEMO}")
        set(_demo_dir "${CMAKE_SOURCE_DIR}/demo/${MGL_DEMO}")
        if(EXISTS "${_demo_dir}/CMakeLists.txt")
            add_subdirectory("${_demo_dir}")
            set(_demo_target "mgl_demo_${MGL_DEMO}")
            if(NOT TARGET ${_demo_target})
                message(FATAL_ERROR "Target ${_demo_target} not defined in ${_demo_dir}/CMakeLists.txt")
            endif()
        else()
            message(FATAL_ERROR "Demo directory ${_demo_dir} does not contain CMakeLists.txt")
        endif()
    endif()

    add_executable(${PROJECT_NAME} ${MGL_APP_SOURCES})
    target_link_libraries(${PROJECT_NAME} PRIVATE mgl_core mgl_platform)
    if(MGL_DEMO)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${_demo_target})
    endif()
endif()

add_custom_target(menuconfig
        COMMAND ${Python_EXECUTABLE} ${KCONFIG_PY} menuconfig
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        USES_TERMINAL
)
add_custom_target(alldefconfig
        COMMAND ${Python_EXECUTABLE} ${KCONFIG_PY} alldefconfig
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

set(MGL_FONT_STAMP "${CMAKE_SOURCE_DIR}/generated/font/.stamp")

if(MGL_FONT_GEN_SCAN_PATH)
    set(_SCAN_ARG "--scan-path" "${MGL_FONT_GEN_SCAN_PATH}")
else()
    set(_SCAN_ARG "")
endif()

add_custom_target(fonts
        COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/font_gen.py ${_SCAN_ARG}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tools
        COMMENT "Generating font files from source code"
)

add_custom_command(
        OUTPUT ${MGL_FONT_STAMP}
        COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/font_gen.py ${_SCAN_ARG}
        COMMAND ${CMAKE_COMMAND} -E touch ${MGL_FONT_STAMP}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tools
        DEPENDS ${CMAKE_SOURCE_DIR}/tools/font_gen.py ${CMAKE_SOURCE_DIR}/.config
        COMMENT "Auto-generating font files (stamp)"
)
add_custom_target(fonts_auto DEPENDS ${MGL_FONT_STAMP})

add_dependencies(mgl_core fonts_auto)