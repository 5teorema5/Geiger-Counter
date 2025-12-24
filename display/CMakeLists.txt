cmake_minimum_required(VERSION 3.13)

project(display)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME} PUBLIC
    "${PROJECT_SOURCE_DIR}/include/display/display.h"
    "${PROJECT_SOURCE_DIR}/src/display.c"
)

target_link_libraries(${PROJECT_NAME} 
        pico_stdlib
        hardware_i2c
        )

target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/include")