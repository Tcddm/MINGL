set(MGL_DEMO_HELLO_SOURCES
        main_page.c
)

add_library(mgl_demo_hello STATIC ${MGL_DEMO_HELLO_SOURCES})
target_include_directories(mgl_demo_hello PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mgl_demo_hello PUBLIC mgl_core)
