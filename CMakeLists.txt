cmake_minimum_required(VERSION 3.22.1) # 3.27
project(CosmoArchitecture)

set(CMAKE_CXX_STANDARD 23)

add_executable(CosmoArchitecture src/main.cpp
        src/utils/log.cpp
        src/utils/log.hpp)
