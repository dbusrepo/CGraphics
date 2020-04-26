#PRINT_CUR_BINARY_DIR()

add_subdirectory(screen_x11)
add_subdirectory(app)

add_executable(LinuxGraphics)

target_sources(LinuxGraphics
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/main.c
        )

target_link_libraries(LinuxGraphics
        PRIVATE
            screen_x11
            app
        )