#PRINT_CUR_BINARY_DIR()

add_library(graphics_x11 STATIC "")

target_sources(graphics_x11
        PRIVATE
            screen_x11.c
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/screen_x11.h
            ${CMAKE_CURRENT_LIST_DIR}/graph_app.h
        )

target_compile_options(graphics_x11
        PRIVATE
            "${MY_OPTIONS}"
            "$<$<CONFIG:Debug>:${MY_DEBUG_OPTIONS}>"
            "$<$<CONFIG:Release>:${MY_RELEASE_OPTIONS}>"
        )

target_include_directories(graphics_x11
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}
        )

target_link_libraries(graphics_x11
        PRIVATE
            X11
            Xext
            Xt
        )