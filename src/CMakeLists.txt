#PRINT_CUR_BINARY_DIR()

add_subdirectory(graphics_utils)
add_subdirectory(screen_x11)
add_subdirectory(app)

#add_subdirectory(_apps)
add_executable(GraphicsApp test_graphics_app.c)

target_link_libraries(GraphicsApp
        PRIVATE
        app
        )

add_custom_target(graphviz ALL
        COMMAND ${CMAKE_COMMAND} "--graphviz=foo.dot" .
        COMMAND dot -Tpng foo.dot -o foo.png
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        )

