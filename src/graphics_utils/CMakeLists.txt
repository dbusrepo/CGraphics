add_library(graphics_utils OBJECT "")

target_sources(graphics_utils
        PRIVATE
        common.h
        font.h
        font.c
        screen_info_rgb.h
        screen_info_rgb_p.h
        screen_info_rgb.c
        )