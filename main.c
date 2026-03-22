#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <locale.h>
#include "headers/scan_engine.h"
#include "headers/config_manager.h"
#include "headers/utils.h"

void print_help(char* param);

void test_and_read_config_file(bool verbose, char *config_dir);

/**
 * Entry point of the application. Parses command-line arguments, processes the input directory,
 * and performs the main scanning and configuration tasks. Provides logging and help functionality.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments (including the program name as the first element).
 *             Supported options:
 *             - '-h': Displays usage help and exits.
 *             - '-v': Enables verbose mode for detailed logging.
 *             - '-x': Prevents the terminal window from closing automatically after execution.
 *             - '-i <input_dir>': Specifies the input directory to process.
 *             - '-c <config_dir>': Specifies the directory for the configuration file.
 * @return An integer indicating the program's exit status. Returns 0 on successful execution,
 *         or exits with an error code in case of a failure or invalid input.
 */
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
    else {
        fprintf(stderr, "There are not valid arguments\n");
        print_help(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (input_dir!=NULL && is_directory(input_dir)) {
        print_help(argv[0]);

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
        fprintf(stderr, "Input dir does not exist!\n");
        print_help(argv[0]);
        exit(EXIT_FAILURE);
    }

}

/**
 * Reads the configuration file from the specified directory or default locations.
 * If a directory is provided, attempts to read the configuration file from that directory.
 * If no directory is provided, it searches for "config.ini" in the current and parent directories.
 * Logs the process if verbose mode is enabled.
 *
 * @param verbose Enables verbose logging if set to true.
 * @param config_dir The directory where the configuration file is expected to be located.
 *                   Pass NULL to use the default search paths.
 */
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
    fprintf(stdout, "\n"
        " __  ______            _   ____                  \n"
        " \\ \\/ /  _ \\  __ _ _ _| |_/ ___|  ___ __ _ _ __  \n"
        "  \\  /| | | |/ _` | '__| __\\___ \\ / __/ _` | '_ \\ \n"
        "  /  \\| |_| | (_| | |  | |_ ___) | (_| (_| | | | |\n"
        " /_/\\_\\____/ \\__,_|_|   \\__|____/ \\___\\__,_|_| |_|\n"
        "                                          v. 1.2\n\n");
    fprintf(stdout, "Usage: %s -i <dir_to_scan> -c <config_file_path> -v\n", param);
    fprintf(stdout, "-i <dir_to_scan>\n");
    fprintf(stdout, "-c <config_file_path>\n");
    fprintf(stdout, "-v: verbose mode\n");
    fprintf(stdout, "-x: not close terminal\n");
}





