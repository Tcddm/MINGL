set(PLATFORM_SOURCES
        hal_sdl.c
)

add_library(mgl_platform STATIC ${PLATFORM_SOURCES})
target_include_directories(mgl_platform PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(mgl_platform PRIVATE ${CMAKE_SOURCE_DIR}/platform/sdl)

if(WIN32)
    set(SDL2_ROOT ${CMAKE_SOURCE_DIR}/platform/sdl)
    set(SDL2_DIR ${SDL2_ROOT}/lib/cmake)
endif()

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(mgl_platform PUBLIC SDL2::SDL2main SDL2::SDL2)