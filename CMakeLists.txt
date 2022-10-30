cmake_minimum_required(VERSION 3.23)
project(xdartscan C)

set(CMAKE_C_STANDARD 99)
add_compile_options(-gdwarf-4)

add_executable(xdartscan
        main.c
        logic/scan_engine.c
        headers/scan_engine.h
        logic/random_test.c
        headers/file_signatures.h
        headers/config.h
        logic/config_manager.c
        headers/config_manager.h
        misc/utils.c
        headers/utils.h
        logic/report_manager.c
        headers/report_manager.h
        headers/random_test.h)
add_executable(xdartscan_test main_test.c logic/scan_engine.c headers/scan_engine.h logic/random_test.c headers/config.h logic/config_manager.c headers/config_manager.h misc/utils.c headers/utils.h logic/report_manager.c headers/report_manager.h headers/random_test.h)