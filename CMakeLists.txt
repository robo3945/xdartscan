cmake_minimum_required(VERSION 3.23)
project(xdartscan C)

set(CMAKE_C_STANDARD 99)

# static link for mingw libraries
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static")
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

add_executable(test_signatures
        testing/test_signatures.c
        logic/config_manager.c
        logic/scan_engine.c
        logic/report_manager.c
        logic/random_test.c
        misc/utils.c
        headers/file_signatures.h)

# https://coderefinery.github.io/cmake-workshop/testing/
# enable testing functionality
enable_testing()

add_test(NAME test_signatures COMMAND test_signatures)