cmake_minimum_required(VERSION 3.29)
project(sudoFun VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

list(SORT SOURCES)
list(SORT HEADERS)

find_package(Qt5 QUIET COMPONENTS Core Widgets)

add_executable(sudofun ${SOURCES} ${HEADERS})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(sudofun PRIVATE DEBUG_BUILD=1)
endif()


if (Qt5_FOUND)
    add_compile_definitions(sudofun PRIVATE BUILT_WITH_QT5=1)
    target_link_libraries(sudofun PRIVATE Qt5::Core Qt5::Widgets)
else()
    message(STATUS "Qt5 not found, executable will be console only. To build GUI, please install Qt5.")
endif()

