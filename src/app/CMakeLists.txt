#PRINT_CUR_BINARY_DIR()

add_library(app OBJECT "") # object library, see $<TARGET_OBJECTS:objLib>

target_sources(app
        PRIVATE
            graph_app.c
        #PUBLIC
        #    ${CMAKE_CURRENT_LIST_DIR}/../screen_x11/graph_app.h
        )

# TODO
target_link_libraries(app
        PRIVATE
            screen_x11
        )