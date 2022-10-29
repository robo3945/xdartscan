#include <string.h>
#include <stdio.h>
#include "headers/scan_engine.h"
#include "headers/config_manager.h"
#include "headers/config.h"
#include "headers/file_signatures.h"


void test_config_read();
void test_entropy();
void sort_signs();

int main(int argc, char *argv[])
{
    //test_entropy();
    //test_config_read();
    sort_signs();

}

void sort_signs() {
    sort_signatures(well_known_magic_number);
}

void test_config_read() {
    if (read_config_file("config.ini"))
        read_config_file("../config.ini");
    printf("Entropy is: %f, Debug is: %d", ENTROPY_TH, DEBUG_PRINT);
}


void test_entropy() {
    unsigned char* s = "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";
    //char* s = "abcdefghilmnopqrstuvzxy1234567890";

    double H= calc_rand_idx(s, strlen(s));

    if (H-4.022379<0.00001)
        printf("H: %f - SUCCESS", H);
    else
        printf("H: %f - KO!", H);
}

