#PRINT_CUR_BINARY_DIR()

add_subdirectory(graphics_utils)
add_subdirectory(screen_x11)
add_subdirectory(app)

add_executable(LinuxGraphics stb_lib/stb_image_write.h stb_lib/stb_ds.h stb_lib/stb_image_resize.h stb_lib/stb_truetype.h stb_lib/stb_sprintf.h)

target_sources(LinuxGraphics
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/main.c
        )

add_custom_target(graphviz ALL
        COMMAND ${CMAKE_COMMAND} "--graphviz=foo.dot" .
        COMMAND dot -Tpng foo.dot -o foo.png
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        )

target_link_libraries(LinuxGraphics
        PRIVATE
            graphics_utils
            screen_x11
            app
        )