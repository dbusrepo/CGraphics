add_library(graphics_utils STATIC "" common.c)

file(GLOB_RECURSE SOURCES "*.c")
file(GLOB_RECURSE HEADERS "*.h")

target_sources(graphics_utils
        PRIVATE
        "${SOURCES}"
        PUBLIC
        "${HEADERS}"
        )

target_compile_options(graphics_utils
        PRIVATE
        "${MY_OPTIONS}"
        "$<$<CONFIG:Debug>:${MY_DEBUG_OPTIONS}>"
        "$<$<CONFIG:Release>:${MY_RELEASE_OPTIONS}>"
        )

target_include_directories(graphics_utils
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        )
        
target_link_libraries(graphics_utils
        PRIVATE
        )

foreach (SOURCE ${SOURCES})
    get_filename_component(SOURCE_NAME "${SOURCE}" NAME)
    #    add_custom_command(TARGET graphics_utils
    #            POST_BUILD
    #            COMMAND make ARGS ${SOURCE_NAME}.s
    #            )
endforeach (SOURCE)