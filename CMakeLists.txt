cmake_minimum_required(VERSION 3.12)
project(Image)

set(CMAKE_CXX_STANDARD 14)

add_executable(Image main.cpp)

target_link_libraries(Image -ljpeg -lIL)
