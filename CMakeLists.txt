cmake_minimum_required (VERSION 2.8)

project (SandtrayLogAnalysis)

#setting output paths
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

set (SOURCE_FILES ${SRC_DIR}/sandtrayLogAnalysis.cpp ${SRC_DIR}/helperFuncs.cpp)
include_directories(${SRC_DIR})

add_executable(SandtrayLogAnalysis ${SOURCE_FILES})
