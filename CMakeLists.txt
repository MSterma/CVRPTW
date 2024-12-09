cmake_minimum_required(VERSION 3.29)
project(CVRPTW)

set(CMAKE_CXX_STANDARD 20)

add_executable(CVRPTW main.cpp
        Client.cpp
        Client.h
        Vehicle.cpp
        Vehicle.h
        solution.cpp
        solution.h)
