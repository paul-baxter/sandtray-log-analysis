cmake_minimum_required (VERSION 2.8)

project (SandtrayLogAnalysis)

#setting output paths
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_executable(SandtrayLogAnalysis ${PROJECT_SOURCE_DIR}/src/sandtrayLogAnalysis.cpp)
