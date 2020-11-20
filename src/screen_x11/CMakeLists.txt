#PRINT_CUR_BINARY_DIR()

add_library(screen_x11 STATIC "")

file(GLOB_RECURSE SOURCES "*.c")
file(GLOB_RECURSE HEADERS "*.h")

target_sources(screen_x11
        PRIVATE
        "${SOURCES}"
        PUBLIC
        "${HEADERS}"
        )

target_compile_options(screen_x11
        PRIVATE
        "${MY_OPTIONS}"
        "$<$<CONFIG:Debug>:${MY_DEBUG_OPTIONS}>"
        "$<$<CONFIG:Release>:${MY_RELEASE_OPTIONS}>"
        )

target_include_directories(screen_x11
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        )

target_link_libraries(screen_x11
        PRIVATE
        X11
        Xext
        Xt
        Xxf86vm
        graphics_utils
        #Xrandr
        #        graphics_utils
        )

foreach (SOURCE ${SOURCES})
    get_filename_component(SOURCE_NAME "${SOURCE}" NAME)
    add_custom_command(TARGET screen_x11
            POST_BUILD
            COMMAND make ARGS ${SOURCE_NAME}.s
            )
endforeach (SOURCE)

