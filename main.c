#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "headers/scan_engine.h"

void print_help(char* idir);

void print_version();


int main(int argc, char *argv[])
{
    bool isCaseInsensitive = false;
    int opt;
    char* idir;

    enum { CHARACTER_MODE, WORD_MODE, LINE_MODE } mode = CHARACTER_MODE;

    if (argc >1)
        while ((opt = getopt(argc, argv, "hvi:")) != -1) {
            switch (opt) {
                case 'i':
                    idir = optarg;
                    scan(optarg);
                    break;
                case 'h':
                    print_help(argv[0]);
                case 'v':
                    print_version();
                default:
                    print_help(argv[0]);
            }
        }
    else
        print_help(argv[0]);

    // Now optind (declared extern int by <unistd.h>) is the index of the first non-option argument.
    // If it is >= argc, there were no non-option arguments.

    // ...
}

void print_help(char *idir) {
    fprintf(stdout, "Usage: %s [-i] [dir_to_scan]\n", idir);
    exit(EXIT_SUCCESS);
}

void print_version() {
    fprintf(stdout, "version: 0.1\n");
    exit(EXIT_SUCCESS);
}





