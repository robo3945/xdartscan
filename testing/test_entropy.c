#include <string.h>
#include <stdio.h>
#include "../headers/random_test.h"

int test_entropy() {
    unsigned char* s = "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";
    //char* s = "abcdefghilmnopqrstuvzxy1234567890";

    double H= calc_rand_idx(s, (long) strlen(s));

    if (H-4.022379<0.00001) {
        printf("H: %f - SUCCESS", H);
        return 0;
    }
    else {
        printf("H: %f - KO!", H);
        return 1;
    }
}

int main(int argc, char *argv[])
{
    return test_entropy();
}
