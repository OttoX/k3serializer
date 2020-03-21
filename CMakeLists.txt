cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(base LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND _sources k3serializer.cpp)
add_executable(example example/main.cpp ${_sources})