#include <string.h>
#include <stdio.h>
#include "headers/scan_engine.h"


int main(int argc, char *argv[])
{
    unsigned char* s = "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";
    //char* s = "abcdefghilmnopqrstuvzxy1234567890";

    double H= calc_rand_idx(s, strlen(s));

    if (H-4.022379<0.00001)
        printf("H: %f - SUCCESS", H);
    else
        printf("H: %f - KO!", H);
}

