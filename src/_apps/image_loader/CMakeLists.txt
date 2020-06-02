#add_subdirectory(../../graphics_utils)
#add_subdirectory(../../screen_x11)
#add_subdirectory(../../app)

set(_APP "image_loader")

add_library(${_APP} STATIC "")

file(GLOB SOURCES "*.c")
file(GLOB HEADERS "*.h")

target_sources(${_APP}
        PRIVATE
        "${SOURCES}"
        PUBLIC
        "${HEADERS}"
        )

target_compile_options(${_APP}
        PRIVATE
        "${MY_OPTIONS}"
        "$<$<CONFIG:Debug>:${MY_DEBUG_OPTIONS}>"
        "$<$<CONFIG:Release>:${MY_RELEASE_OPTIONS}>"
        )

target_link_libraries(${_APP}
        PRIVATE
        app
        m # math lib
        )

foreach (SOURCE ${SOURCES})
    get_filename_component(SOURCE_NAME "${SOURCE}" NAME)
    add_custom_command(TARGET ${_APP}
            POST_BUILD
            COMMAND make ARGS ${SOURCE_NAME}.s
            )
endforeach (SOURCE)
