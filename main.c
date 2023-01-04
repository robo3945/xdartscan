#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <locale.h>
#include "headers/scan_engine.h"
#include "headers/config_manager.h"

void print_help(char* param);

void test_and_read_config_file(bool verbose, char *config_dir);

int main(int argc, char *argv[])
{
    // Set locale for floating nums representation
    setlocale(LC_NUMERIC, ".OCP");
    int opt;
    bool verbose = false;
    bool not_close_terminal_window = false;
    char* input_dir = NULL;
    char* config_dir = NULL;
    if (argc >1)
        while ((opt = getopt(argc, argv, "hvxi:c:")) != -1) {
            switch (opt) {
                case 'v':
                    verbose = true;
                    continue;
                case 'x':
                    not_close_terminal_window = true;
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
        test_and_read_config_file(verbose, config_dir);
        main_scan(input_dir, verbose);

        if (verbose) {
            // Only to show the configuration params at the end of computation (for verbose mode)
            test_and_read_config_file(verbose, config_dir);
        }

        if (not_close_terminal_window) {
            printf("\n\n\nPress RETURN to close...");
            getc(stdin);
        }
    }
    else {
        print_help(argv[0]);
    }

}

void test_and_read_config_file(bool verbose, char *config_dir) {
    if (config_dir == NULL) {
        if (read_config_file("config.ini", verbose))
            if (read_config_file("../config.ini", verbose))
                printf("Configuration file not found in path: \"%s\"", "../config.ini");
    }
    else if (read_config_file(config_dir, verbose))
        printf("Configuration file not found in path: \"%s\"", config_dir);
}

/**
 * The help
 * @param param
 */
void print_help(char *param) {
    fprintf(stdout, "XDartScan v. 1.0b\n");
    fprintf(stdout, "Usage: %s -i <dir_to_scan> -c <config_file_path -v\n", param);
    fprintf(stdout, "-i <dir_to_scan>\n");
    fprintf(stdout, "-c <config_file_path>\n");
    fprintf(stdout, "-v: verbose mode\n");
    fprintf(stdout, "-x: not close terminal\n");

    exit(EXIT_SUCCESS);
}





