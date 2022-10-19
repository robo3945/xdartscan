#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "headers/scan_engine.h"


void convert_string_to_int_array(const unsigned char* s, int* buffer_out)
{
    for (int i=0;i<strlen(s); i++)
      buffer_out[i]=s[i];
}


int main(int argc, char *argv[])
{
    unsigned char* s = "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";
    //char* s = "abcdefghilmnopqrstuvzxy1234567890";

    double H= calc_rand_idx(s, strlen(s), true);

    if (H-4.022379<0.00001)
        printf("H: %f - SUCCESS", H);
    else
        printf("H: %f - KO!", H);
}

