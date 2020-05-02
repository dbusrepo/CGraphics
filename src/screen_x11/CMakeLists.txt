#PRINT_CUR_BINARY_DIR()

add_library(screen_x11 OBJECT "")

target_sources(screen_x11
        PRIVATE
        screen.c
        ../graphics_utils/common.h
        ../graphics_utils/screen_info_rgb.h
        ../graphics_utils/screen_info_rgb_p.h
        ../graphics_utils/screen_settings.h
        ../graphics_utils/screen_callback.h
        ../graphics_utils/input_keys.h
        ../app/screen.h
        PUBLIC
        ../app/screen.h
        ../graphics_utils/screen_info_rgb.h
        ../graphics_utils/screen_settings.h
        ../graphics_utils/screen_callback.h
        ../graphics_utils/input_keys.h
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
        #Xrandr
        #        graphics_utils
        )
