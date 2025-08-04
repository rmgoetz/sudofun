cmake_minimum_required(VERSION 3.29)
project(sudoFun VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

file(GLOB MAIN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
file(GLOB BENCH_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
list(REMOVE_ITEM MAIN_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/benchmark.cpp"
)
list(REMOVE_ITEM BENCH_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/benchmark.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/window.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/window.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/run.hpp"
)

find_package(Qt5 QUIET COMPONENTS Core Widgets)

add_executable(sudofun "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp" ${MAIN_SOURCES})
add_executable(sudofun_benchmark "${CMAKE_CURRENT_SOURCE_DIR}/src/benchmark.cpp" ${BENCH_SOURCES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(sudofun PRIVATE DEBUG_BUILD=1)
    add_compile_definitions(sudofun_benchmark PRIVATE DEBUG_BUILD=1)
endif()

if (Qt5_FOUND)
    message(STATUS "Qt5 package found!")
    message(STATUS "Qt5Core_FOUND: ${Qt5Core_FOUND}")
    message(STATUS "Qt5Widgets_FOUND: ${Qt5Widgets_FOUND}")
    message(STATUS "Qt5_DIR: ${Qt5_DIR}")
    message(STATUS "Qt5Core_INCLUDE_DIRS: ${Qt5Core_INCLUDE_DIRS}")
    message(STATUS "Qt5Widgets_INCLUDE_DIRS: ${Qt5Widgets_INCLUDE_DIRS}")
    add_compile_definitions(sudofun PRIVATE BUILT_WITH_QT5=1)
    target_link_libraries(sudofun PRIVATE Qt5::Core Qt5::Widgets)
else()
    message(STATUS "Qt5 not found, executable will be console only. To build GUI, please install Qt5.")
endif()