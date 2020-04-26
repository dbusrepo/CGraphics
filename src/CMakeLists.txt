#PRINT_CUR_BINARY_DIR()

add_subdirectory(screen_x11)
add_subdirectory(app)

add_executable(XlibGraphics)

target_sources(XlibGraphics
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/main.c
        )

target_link_libraries(XlibGraphics
        PRIVATE
            screen_x11
            app
        )