add_library(app "")

target_sources(app
        PRIVATE
            graph_app.c
        #PUBLIC
        #    ${CMAKE_CURRENT_LIST_DIR}/../graphics_x11/graph_app.h
        )

target_link_libraries(app
        PRIVATE
            graphics_x11
        )