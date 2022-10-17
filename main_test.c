#include <stdbool.h>
#include "headers/scan_engine.h"


int main(int argc, char *argv[])
{
    char* s = "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";
    //char* s = "abcdef";
    calc_rand_idx(s, true);
}

