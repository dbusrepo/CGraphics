#PRINT_CUR_BINARY_DIR()

add_subdirectory(image_loader)
add_subdirectory(ray_tracer)

#add_executable(LinuxGraphics)
#
#target_sources(LinuxGraphics
#        PRIVATE
#        _apps/image_loader/main.c
#        )
#
#add_custom_target(graphviz ALL
#        COMMAND ${CMAKE_COMMAND} "--graphviz=foo.dot" .
#        COMMAND dot -Tpng foo.dot -o foo.png
#        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
#        )
#
#target_link_libraries(LinuxGraphics
#        PRIVATE
#        image_loader
#        )