cmake_minimum_required(VERSION 3.23)
project(xdartscan C)

set(CMAKE_C_STANDARD 99)

add_executable(xdartscan main.c logic/scan_engine.c headers/scan_engine.h logic/random_test.c headers/file_signatures.h headers/config.h misc/config_manager.c misc/config_manager.h)
add_executable(xdartscan_test main_test.c logic/scan_engine.c headers/scan_engine.h logic/random_test.c headers/config.h misc/config_manager.c misc/config_manager.h)