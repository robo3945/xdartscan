#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "headers/scan_engine.h"
#include "headers/config_manager.h"

void print_help(char* param);

int main(int argc, char *argv[])
{
    int opt;
    bool verbose = false;
    char* input_dir = NULL;
    char* config_dir = NULL;
    if (argc >1)
        while ((opt = getopt(argc, argv, "hvi:c:")) != -1) {
            switch (opt) {
                case 'v':
                    verbose = true;
                    continue;
                case 'i':
                    input_dir = optarg;
                    continue;
                case 'c':
                    config_dir = optarg;
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

    if (input_dir!=NULL) {
        if (config_dir == NULL) {
            if (read_config_file("config.ini", verbose))
                read_config_file("../config.ini", verbose);
        }
        else
            read_config_file(config_dir, verbose);

        main_scan(input_dir, verbose);
    }
    else {
        print_help(argv[0]);
    }

}

void print_help(char *param) {
    fprintf(stdout, "XDartScan v. 1.0b\n");
    fprintf(stdout, "Usage: %s -i <dir_to_scan> -c <config_file_path>\n", param);
    exit(EXIT_SUCCESS);
}





