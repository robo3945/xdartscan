#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "headers/scan_engine.h"

void print_help(char* idir);

int main(int argc, char *argv[])
{
    int opt;
    bool verbose = false;
    char* input_dir = NULL;
    if (argc >1)
        while ((opt = getopt(argc, argv, "hvi:")) != -1) {
            switch (opt) {
                case 'v':
                    verbose = true;
                    continue;
                case 'i':
                    input_dir = optarg;
                    continue;
                case 'h':
                    print_help(argv[0]);
                    exit(EXIT_SUCCESS);
                default:
                    print_help(argv[0]);
                    exit(EXIT_FAILURE);
            }
        }
    else
        print_help(argv[0]);

    if (input_dir!=NULL)
        main_scan(input_dir, verbose);

}

void print_help(char *idir) {
    fprintf(stdout, "XDartScan v. 1.0b\n");
    fprintf(stdout, "Usage: %s [-i] [dir_to_scan] [-x]\n", idir);
    exit(EXIT_SUCCESS);
}





