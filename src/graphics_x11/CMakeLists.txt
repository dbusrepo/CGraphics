add_library(graphics_x11 "")

target_sources(graphics_x11
        PRIVATE
            screen_x11.c
        PUBLIC
            ${CMAKE_CURRENT_LIST_DIR}/screen_x11.h
            ${CMAKE_CURRENT_LIST_DIR}/graph_app.h
        )

target_include_directories(graphics_x11 PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(graphics_x11 PRIVATE X11)
target_link_libraries(graphics_x11 PRIVATE Xext)
target_link_libraries(graphics_x11 PRIVATE Xt)