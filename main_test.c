#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "headers/scan_engine.h"


void convert_string_to_int_array(const char* s, int* buffer_out)
{
    for (int i=0;i<strlen(s); i++)
      buffer_out[i]=s[i];
}


int main(int argc, char *argv[])
{
    //char* s = "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";
    char* s = "abcdef";

    int *buffer = malloc(strlen(s)*sizeof(int));
    convert_string_to_int_array(s, buffer);
    calc_rand_idx(buffer, strlen(s), true);
    free(buffer);

}

