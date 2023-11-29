cmake_minimum_required(VERSION 3.10)
project(seam_smooth)

if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_compile_options("-O2")
endif()


find_package(OpenCV REQUIRED)
find_package(fmt REQUIRED)

set(ThirdLibs "${OpenCV_LIBRARIES}")


include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/test)

file(GLOB SrcFiles ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(test 
    ${PROJECT_SOURCE_DIR}/test/test.cpp
    ${SrcFiles}
)
target_link_libraries(test ${ThirdLibs} fmt::fmt)
