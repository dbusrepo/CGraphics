message("current binary dir is ${CMAKE_CURRENT_BINARY_DIR}")

add_executable(XlibGraphics main.c)

add_subdirectory(graphics_x11)
add_subdirectory(app)

target_link_libraries(XlibGraphics
        PRIVATE
            graphics_x11
            app
        )