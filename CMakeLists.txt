cmake_minimum_required(VERSION 3.15)
project(XlibGraphics LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(gcc_options -m64 -pedantic-errors -Wall -Wextra -Wshadow
-Wundef -fno-common -Wconversion -Wcast-qual -Wwrite-strings -Wlogical-op
-Wfloat-equal -Wmissing-prototypes -Wstrict-prototypes -Wredundant-decls
-Wcast-align -Wdisabled-optimization
-Wno-unused-parameter)

set(MY_OPTIONS)
set(MY_DEBUG_OPTIONS)
set(MY_RELEASE_OPTIONS)

#-pedantic-errors

message("using cmake ${CMAKE_VERSION}")

#https://stackoverflow.com/questions/10046114/in-cmake-how-can-i-test-if-the-compiler-is-clang
if(CMAKE_C_COMPILER_ID MATCHES GNU)
    # using GCC
    message("using gcc " ${CMAKE_C_COMPILER_VERSION})
    list(APPEND MY_OPTIONS ${gcc_options}) #${gcc_options}
    list(APPEND MY_DEBUG_OPTIONS "-Og")
    list(APPEND MY_RELEASE_OPTIONS "-O3")
endif()

if(CMAKE_C_COMPILER_ID MATCHES Intel)
    # using GCC
    message("using icc " ${CMAKE_C_COMPILER_VERSION})
    #list(APPEND MY_OPTIONS ${gcc_options}) #${gcc_options}
    #list(APPEND MY_DEBUG_OPTIONS "-Og")
    #list(APPEND MY_RELEASE_OPTIONS "-O3")
endif()


#https://stackoverflow.com/questions/31790467/how-to-enable-all-compiler-warnings-in-clion
#set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

#set(CMAKE_C_FLAGS_DEBUG "-g -Og")
#set(CMAKE_C_FLAGS_RELEASE "-O3") # see -Wno-unused

# the compiler flags for compiling C sources
#message( STATUS "CMAKE_C_FLAGS: " ${CMAKE_C_FLAGS} )
#message( STATUS "CMAKE_C_FLAGS_DEBUG: " ${CMAKE_C_FLAGS_DEBUG} )
#message( STATUS "CMAKE_C_FLAGS_RELEASE: " ${CMAKE_C_FLAGS_RELEASE} )

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

message("build type: " ${CMAKE_BUILD_TYPE})

#https://stackoverflow.com/questions/25193239/cmake-directory-hierarchy-building-a-sub-directory-independently
# -Dproject1=1 or the whole project with -DALL=1.
MACRO(ADD_PROJECT PROJECT)
    IF(${PROJECT} OR ALL)
        add_subdirectory(${PROJECT})
    ENDIF()
ENDMACRO()

macro(PRINT_CUR_BINARY_DIR)
    message("current binary dir is ${CMAKE_CURRENT_BINARY_DIR}")
endmacro()

add_subdirectory(src)
#include ext_libs



