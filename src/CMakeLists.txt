#PRINT_CUR_BINARY_DIR()

add_subdirectory(graphics_utils)
add_subdirectory(screen_x11)
add_subdirectory(app)

#add_subdirectory(_apps)
add_executable(x11BaseApp x11BaseApp.c)

target_compile_options(x11BaseApp
        PRIVATE
        "${MY_OPTIONS}"
        "$<$<CONFIG:Debug>:${MY_DEBUG_OPTIONS}>"
        "$<$<CONFIG:Release>:${MY_RELEASE_OPTIONS}>"
        )
        
target_link_libraries(x11BaseApp
        PRIVATE
		"$<$<CONFIG:Debug>:asan>"
        "$<$<CONFIG:Debug>:ubsan>"
        app
        )

add_custom_target(graphviz ALL
        COMMAND ${CMAKE_COMMAND} "--graphviz=foo.dot" .
        COMMAND dot -Tpng foo.dot -o foo.png
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        )

