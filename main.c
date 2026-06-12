#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <locale.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <glob.h>
#endif
#include "headers/scan_engine.h"
#include "headers/config_manager.h"
#include "headers/config.h"
#include "headers/utils.h"

void print_help(char* param);
void clean_reports(void);
void test_and_read_config_file(bool verbose, char *config_path);

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
    char* config_path = NULL;
    char* json_path = NULL;
    // Handle long-style -clean argument before getopt
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-clean") == 0) {
            clean_reports();
            exit(EXIT_SUCCESS);
        }
    }

    if (argc >1)
        while ((opt = getopt(argc, argv, "hvxi:c:t:j:")) != -1) {
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
                    config_path = optarg;
                    continue;
                case 't':
                    set_num_threads_from_cli((int) strtol(optarg, NULL, 10));
                    continue;
                case 'j':
                    json_path = optarg;
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

        test_and_read_config_file(verbose, config_path);
        // CLI value takes precedence over config.ini
        main_scan(input_dir, verbose, json_path);

        if (verbose) {
            // Only to show the configuration params at the end of computation (for verbose mode)
            test_and_read_config_file(verbose, config_path);
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
void test_and_read_config_file(bool verbose, char *config_path) {
    if (config_path == NULL) {
        if (read_config_file("config.ini", verbose))
            if (read_config_file("../config.ini", verbose))
                printf("Configuration file not found in path: \"%s\"", "../config.ini");
    } else if (is_directory(config_path)) {
        char path[MAX_PATH_BUFFER];
        snprintf(path, sizeof(path), "%s/config.ini", config_path);
        if (read_config_file(path, verbose))
            printf("Configuration file not found in path: \"%s\"", path);
    } else {
        if (read_config_file(config_path, verbose))
            printf("Configuration file not found in path: \"%s\"", config_path);
    }
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
        "                                          v. 1.3\n\n");
    fprintf(stdout, "Usage: %s -i <dir_to_scan> -c <config_file_path> -v\n", param);
    fprintf(stdout, "-i <dir_to_scan>\n");
    fprintf(stdout, "-c <config_file_path>\n");
    fprintf(stdout, "-v: verbose mode\n");
    fprintf(stdout, "-x: not close terminal\n");
    fprintf(stdout, "-t <n_threads>: set number of worker threads (overrides config.ini)\n");
    fprintf(stdout, "-j <file.json>: write JSON report to the specified file\n");
    fprintf(stdout, "-clean: delete all report and stats files in the current directory\n");
}

/**
 * Deletes all report (report*.tsv) and stats (stats*.txt) files in the current directory.
 */
void clean_reports(void) {
    int count = 0;

#ifdef _WIN32
    const char *patterns[] = {".\\report*.tsv", ".\\stats*.txt"};
    for (int i = 0; i < 2; i++) {
        WIN32_FIND_DATAA fd;
        HANDLE h = FindFirstFileA(patterns[i], &fd);
        if (h != INVALID_HANDLE_VALUE) {
            do {
                char path[MAX_PATH];
                snprintf(path, sizeof(path), ".\\%s", fd.cFileName);
                if (remove(path) == 0) {
                    printf("Deleted: %s\n", path);
                    count++;
                } else {
                    fprintf(stderr, "Failed to delete: %s\n", path);
                }
            } while (FindNextFileA(h, &fd));
            FindClose(h);
        }
    }
#else
    glob_t g;
    const char *patterns[] = {"./report*.tsv", "./stats*.txt"};
    for (int i = 0; i < 2; i++) {
        if (glob(patterns[i], 0, NULL, &g) == 0) {
            for (size_t j = 0; j < g.gl_pathc; j++) {
                if (remove(g.gl_pathv[j]) == 0) {
                    printf("Deleted: %s\n", g.gl_pathv[j]);
                    count++;
                } else {
                    fprintf(stderr, "Failed to delete: %s\n", g.gl_pathv[j]);
                }
            }
            globfree(&g);
        }
    }
#endif

    printf("%d file(s) deleted.\n", count);
}





