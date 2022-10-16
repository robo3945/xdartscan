cmake_minimum_required(VERSION 3.23)
project(xdartscan C)

set(CMAKE_C_STANDARD 99)

add_executable(xdartscan main.c scan_engine.c headers/scan_engine.h)
