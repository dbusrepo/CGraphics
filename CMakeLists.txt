cmake_minimum_required(VERSION 3.15)
project(CGraphics LANGUAGES C)

#set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_STANDARD 11)
#set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_C_EXTENSIONS OFF)

message(STATUS "CMAKE_SYSTEM_NAME        = ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_VERSION            = ${CMAKE_VERSION}")
message(STATUS "CMAKE_C_COMPILER_ID      = ${CMAKE_C_COMPILER_ID}")
message(STATUS "CMAKE_C_COMPILER_VERSION = ${CMAKE_C_COMPILER_VERSION}")

#https://gcc.gnu.org/onlinedocs/gcc/Option-Summary.html
set(MY_GCC_OPTIONS -m64 -pedantic-errors -Wall -Wextra -Wshadow
        -Wundef -fno-common -Wconversion -Wcast-qual -Wwrite-strings -Wlogical-op
        -Wfloat-equal -Wredundant-decls
        -Wcast-align -Wdisabled-optimization
        -Wno-unused-parameter -fverbose-asm -masm=intel) #-Wmissing-prototypes -Wstrict-prototypes

#https://stackoverflow.com/questions/27343360/how-to-turn-on-icc-icpc-warnings
set(MY_ICC_OPTIONS -w3 -diag-disable:remark -Wall -Warray-bounds -Wchar-subscripts -Wcomment -Wenum-compare -Wformat -Wuninitialized -Wmaybe-uninitialized -Wmain -Wnarrowing -Wnonnull -Wparentheses -Wpointer-sign -Wreorder -Wreturn-type -Wsign-compare -Wsequence-point -Wtrigraphs -Wunused-function -Wunused-but-set-variable -Wunused-variable -Wwrite-strings)

#set(MY_OPTIONS)
#set(MY_DEBUG_OPTIONS)
#set(MY_RELEASE_OPTIONS)

#-pedantic-errors


#https://stackoverflow.com/questions/10046114/in-cmake-how-can-i-test-if-the-compiler-is-clang
if (CMAKE_C_COMPILER_ID MATCHES GNU)
    # using GCC
    message("using gcc " ${CMAKE_C_COMPILER_VERSION})
    list(APPEND MY_OPTIONS ${MY_GCC_OPTIONS}) #${gcc_options}
    list(APPEND MY_DEBUG_OPTIONS "-Og")
    list(APPEND MY_RELEASE_OPTIONS "-O3" "-Wno-unused" "-fno-tree-vectorize") # -Wno-unused") #note: "-fno-tree-vectorize" disable vectorization

endif ()

if (CMAKE_C_COMPILER_ID MATCHES Intel)
    # using GCC
    message("using icc " ${CMAKE_C_COMPILER_VERSION})
    list(APPEND MY_OPTIONS ${MY_ICC_OPTIONS}) #${gcc_options}
    list(APPEND MY_DEBUG_OPTIONS "-Og") # "-Og"
    list(APPEND MY_RELEASE_OPTIONS "-O3" "-Wno-unused")
    #list(APPEND MY_OPTIONS ${gcc_options}) #${gcc_options}
    #list(APPEND MY_DEBUG_OPTIONS "-Og")
    #list(APPEND MY_RELEASE_OPTIONS "-O3")
endif ()


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
if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory")
endif ()

message("build type: " ${CMAKE_BUILD_TYPE})

#https://stackoverflow.com/questions/25193239/cmake-directory-hierarchy-building-a-sub-directory-independently
# -Dproject1=1 or the whole project with -DALL=1.
MACRO(ADD_PROJECT PROJECT)
    IF (${PROJECT} OR ALL)
        add_subdirectory(${PROJECT})
    ENDIF ()
ENDMACRO()

macro(PRINT_CUR_BINARY_DIR)
    message("current binary dir is ${CMAKE_CURRENT_BINARY_DIR}")
endmacro()

add_subdirectory(src)
#include ext_libs



