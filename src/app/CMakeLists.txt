#PRINT_CUR_BINARY_DIR()

add_library(app OBJECT "") # object library, see $<TARGET_OBJECTS:objLib>

target_sources(app
        PRIVATE
        app.c
        screen.h
        ../graphics_utils/common.h
        ../graphics_utils/screen_settings.h
        ../graphics_utils/screen_info_rgb.h
        ../graphics_utils/screen_callback.h
        ../graphics_utils/input_keys.h
        ../graphics_utils/font.h
        PUBLIC
        app.h
        ../graphics_utils/screen_settings.h
        ../graphics_utils/screen_callback.h
        ../graphics_utils/input_keys.h
        )

# TODO
target_link_libraries(app
        PRIVATE
        screen_x11
        graphics_utils
        )
