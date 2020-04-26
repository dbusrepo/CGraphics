#PRINT_CUR_BINARY_DIR()

add_library(screen_x11 STATIC "" ../graphics_utils/screen_info_rgb.h ../graphics_utils/screen_info_rgb.c ../graphics_utils/font.h ../graphics_utils/font.c ../graphics_utils/common.h screen_info_rgb_p.h)

target_sources(screen_x11
        PRIVATE
            screen_x11.c
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/screen_x11.h
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
        )