cmake_minimum_required(VERSION 3.24)

project(mission-control)

set(CMAKE_C_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bld)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

add_compile_definitions(ENABLE_ASSERT=1)
add_compile_definitions(DEBUG=1)

include_directories(${PROJECT_SOURCE_DIR})

file(GLOB mission_control_src CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/*.h" "${PROJECT_SOURCE_DIR}/*.c")

add_executable(${PROJECT_NAME}  ${mission_control_src})

