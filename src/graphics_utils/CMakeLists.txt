add_library(graphics_utils OBJECT "" screen_callback.h input_keys.h)

target_sources(graphics_utils
        PRIVATE
        common.h
        font.h
        font.c
        screen_info_rgb.h
        screen_info_rgb_p.h
        screen_info_rgb.c
        )