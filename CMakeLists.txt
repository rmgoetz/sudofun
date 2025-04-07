cmake_minimum_required(VERSION 3.29)
project(sudoFun)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.hpp")

if(NOT SOURCES)
    message(FATAL_ERROR "No source files found in src/")
endif()

list(SORT SOURCES)
list(SORT HEADERS)

add_executable(sudofun ${SOURCES} ${HEADERS})