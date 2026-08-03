set(PLATFORM_SOURCES
        hal_fb.c
)

add_library(mgl_platform STATIC ${PLATFORM_SOURCES})
target_include_directories(mgl_platform PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(mgl_platform PRIVATE ${CMAKE_SOURCE_DIR}/platform/fb)
