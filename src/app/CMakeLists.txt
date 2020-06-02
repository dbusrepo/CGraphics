#PRINT_CUR_BINARY_DIR()

add_library(app STATIC "") # object library, see $<TARGET_OBJECTS:objLib>

file(GLOB SOURCES "*.c")
file(GLOB HEADERS "*.h")

target_sources(app
        PRIVATE
        "${SOURCES}"
        PUBLIC
        "${HEADERS}"
        )

target_compile_options(app
        PRIVATE
        "${MY_OPTIONS}"
        "$<$<CONFIG:Debug>:${MY_DEBUG_OPTIONS}>"
        "$<$<CONFIG:Release>:${MY_RELEASE_OPTIONS}>"
        )

target_include_directories(app
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        )

# TODO
target_link_libraries(app
        PRIVATE
        screen_x11
        graphics_utils
        )

foreach (SOURCE ${SOURCES})
    get_filename_component(SOURCE_NAME "${SOURCE}" NAME)
    add_custom_command(TARGET app
            POST_BUILD
            COMMAND make ARGS ${SOURCE_NAME}.s
            )
endforeach (SOURCE)