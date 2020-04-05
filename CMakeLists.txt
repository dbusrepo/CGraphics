cmake_minimum_required(VERSION 3.15)
project(XlibGraphics C)

set(CMAKE_C_STANDARD 11)

#https://stackoverflow.com/questions/31790467/how-to-enable-all-compiler-warnings-in-clion
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Wpedantic -Werror -Wall -Wextra -Wshadow \
    -Wundef -fno-common -Wconversion -Wcast-qual -Wwrite-strings -Wlogical-op \
    -Wfloat-equal -Wmissing-prototypes -Wstrict-prototypes -Wredundant-decls \
    -Wcast-align -Wdisabled-optimization \
    -Wno-unused-parameter")

#MESSAGE(ciao asdasdsa)

# the compiler flags for compiling C sources
MESSAGE( STATUS "CMAKE_C_FLAGS: " ${CMAKE_C_FLAGS} )  # ma funzia??

#set(SOURCE_FILES main.c)
add_executable(XlibGraphics src/main.c src/lib/screen_x11.h src/lib/screen_x11.c src/lib/graph_app.h src/app/graph_app.c)

target_link_libraries(XlibGraphics X11)
target_link_libraries(XlibGraphics Xext)
target_link_libraries(XlibGraphics Xt)
#target_link_libraries(XLibGraphics m)
