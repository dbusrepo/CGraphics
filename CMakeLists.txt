cmake_minimum_required(VERSION 3.15)
project(XlibGraphics LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

#https://stackoverflow.com/questions/31790467/how-to-enable-all-compiler-warnings-in-clion
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64 -Wpedantic -Werror -Wall -Wextra -Wshadow \
-Wundef -fno-common -Wconversion -Wcast-qual -Wwrite-strings -Wlogical-op \
-Wfloat-equal -Wmissing-prototypes -Wstrict-prototypes -Wredundant-decls \
-Wcast-align -Wdisabled-optimization \
-Wno-unused-parameter")

set(CMAKE_C_FLAGS_DEBUG "-g -Og")
set(CMAKE_C_FLAGS_RELEASE "-O3")

# the compiler flags for compiling C sources
MESSAGE( STATUS "CMAKE_C_FLAGS: " ${CMAKE_C_FLAGS} )
MESSAGE( STATUS "CMAKE_C_FLAGS_DEBUG: " ${CMAKE_C_FLAGS_DEBUG} )
MESSAGE( STATUS "CMAKE_C_FLAGS_RELEASE: " ${CMAKE_C_FLAGS_RELEASE} )

#https://stackoverflow.com/questions/41361631/optimize-in-cmake-by-default

# Set a default build type for single-configuration
# CMake generators if no build type is set.
#IF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
#    SET(CMAKE_BUILD_TYPE Release)
#ENDIF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

# protect your project against in-source builds:
if(${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory")
endif()


add_subdirectory(src)


#set(SOURCE_FILES main.c)
#add_executable(XlibGraphics src/main.c src/graphics_x11/screen_x11.h src/graphics_x11/screen_x11.c src/graphics_x11/graph_app.h src/app/graph_app.c)


#target_link_libraries(XLibGraphics m)
