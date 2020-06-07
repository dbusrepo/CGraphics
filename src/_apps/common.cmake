#add_library(${APP_EXE} STATIC "")

file(GLOB SOURCES "*.c")
file(GLOB HEADERS "*.h")

target_sources(${APP_EXE}
        PRIVATE
        "${SOURCES}"
        PUBLIC
        "${HEADERS}"
        )

target_compile_options(${APP_EXE}
        PRIVATE
        "${MY_OPTIONS}"
        "$<$<CONFIG:Debug>:${MY_DEBUG_OPTIONS}>"
        "$<$<CONFIG:Release>:${MY_RELEASE_OPTIONS}>"
        )

target_link_libraries(${APP_EXE}
        PRIVATE
        app
        m # math lib
        )

#set(INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR} CACHE PATH "Installation directory for executables")
install(TARGETS ${APP_EXE} RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)

foreach (SOURCE ${SOURCES})
    get_filename_component(SOURCE_NAME "${SOURCE}" NAME)
    add_custom_command(TARGET ${APP_EXE}
            POST_BUILD
            COMMAND make ARGS ${SOURCE_NAME}.s
            )
endforeach (SOURCE)