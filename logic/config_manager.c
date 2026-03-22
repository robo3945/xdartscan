#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdbool.h>
#include "../headers/file_signatures.h"
#include "../headers/config_manager.h"
#include "../headers/config.h"
#include "../headers/utils.h"

#define CONFIG_MAXLINE 2048
#define CONFIG_MAXPARAM 256

// Default value
double ENTROPY_TH=7.00;
int DEBUG_PRINT=1;
int THROUGHPUT_TEST=0;
int MIN_FILE_SIZE=500; //bytes
int MAX_FILE_SIZE=10000000; //bytes

// Definition for global g_stats
GlobStat g_stats = {};

void p_populate_struct(MagicNumber *mn_array);

/**
 * Reads a configuration file and parses its contents into respective global parameters.
 *
 * The function interprets key-value pairs in a configuration file, trims the parameters,
 * and updates the appropriate global variables for configuration settings. If verbose
 * mode is enabled, it prints the parsed parameters and their values to the standard output.
 *
 * @param filename The path to the configuration file to be read.
 * @param verbose A flag indicating whether verbose output should be printed (true for verbose, false otherwise).
 * @return Returns 0 if the file is successfully parsed, or 1 if the file could not be opened.
 */
int read_config_file(char* filename, const bool verbose) {
    FILE *fp;
    
    // Open and parse the INI-style config file
    if ((fp = fopen(filename, "r")) != NULL) {
        printf("\n---------------------------- CONFIG ---------------------------- \n");
        printf("Config path: %s\n\n", filename);

        while (!feof(fp)) {
            char line[CONFIG_MAXLINE];
            fgets(line, CONFIG_MAXLINE, fp);

            const char* delim = "=";
            int num_token = 0;
            char *token = strtok(line, delim);
            char* param_name = NULL, *param_value = NULL;
            while( token != NULL ) {
                switch(num_token)
                {
                    case 0:
                        param_name = trim(token);
                        break;

                    case 1:
                        if (param_name) {
                            param_value = trim(token);
                            // Use strcmp for exact key matching (no length limit needed for known keys)
                            if (strcmp(param_name, "ENTROPY_TH") == 0) {
                                ENTROPY_TH = strtod(param_value, NULL);
                                verbose?printf("Config param: %s value: \t\t%f\n","ENTROPY_TH",ENTROPY_TH):0;
                            }
                            else if (strcmp(param_name, "DEBUG_PRINT") == 0) {
                                DEBUG_PRINT = (int) strtol(param_value, NULL, 10);
                                verbose?printf("Config param: %s value: \t\t%d\n","DEBUG_PRINT",DEBUG_PRINT):0;
                            }
                            // THROUGHPUT_TEST
                            else if (strncmp(param_name, "THROUGHPUT_TEST", CONFIG_MAXPARAM - 1) == 0) {
                                THROUGHPUT_TEST = (int) strtol(param_value, NULL, 10);
                                verbose?printf("Config param: %s value: \t\t%d\n","THROUGHPUT_TEST",THROUGHPUT_TEST):0;
                            }
                            else if (strncmp(param_name, "MIN_FILE_SIZE", MIN_FILE_SIZE - 1) == 0) {
                                MIN_FILE_SIZE = (int) strtol(param_value, NULL, 10);
                                verbose?printf("Config param: %s value: \t\t%d\n","MIN_FILE_SIZE",MIN_FILE_SIZE):0;
                            }
                            else if (strncmp(param_name, "MAX_FILE_SIZE", MAX_FILE_SIZE - 1) == 0) {
                                MAX_FILE_SIZE = (int) strtol(param_value, NULL, 10);
                                verbose?printf("Config param: %s value: \t\t%d\n","MAX_FILE_SIZE",MAX_FILE_SIZE):0;
                            }

                        }
                        else
                            fprintf(stderr, "Param name is NULL: %s\n", token);
                        break;

                    default:
                        fprintf(stderr, "Too many tokens in the config file. Check the delimitation char: %s\n", line);
                        break;

                }

                token = strtok(NULL, delim);
                num_token++;
            }

            if (param_name)
                free(param_name);
            if (param_value)
                free(param_value);

        }
        printf("---------------------------- ///// ---------------------------- \n");

        fclose(fp);
    } else {
        return 1;
    }

    return 0;
}

static int compare_magic_numbers(const void *a, const void *b) {
    const MagicNumber *ma = (const MagicNumber *)a;
    const MagicNumber *mb = (const MagicNumber *)b;
    if (ma->number8_ul < mb->number8_ul) return -1;
    if (ma->number8_ul > mb->number8_ul) return 1;
    return 0;
}

/**
 * Sorts an array of MagicNumber structures based on the `number8_ul` attribute in ascending order.
 *
 * @param mn_array A pointer to an array of MagicNumber structures to be sorted.
 */
void sort_signatures(MagicNumber* mn_array){

    // First of all, trim the signatures to first 4 bytes and converts in unsigned long
    p_populate_struct(mn_array);

    // Sort signatures using stdlib qsort — O(n log n) vs previous O(n^2) selection sort
    qsort(mn_array, SIGNATURES_VECTOR_LENGTH, sizeof(MagicNumber), compare_magic_numbers);

    if (DEBUG_PRINT) {
        printf("\n************************************************\n");
        printf("Signatures table print\n");
        for (int i = 0; i < SIGNATURES_VECTOR_LENGTH; ++i) {
            printf("n8_s: %s\t\t\tn8_ul: %lu\t\t-n_s: %s\t\t-types: %s\n", mn_array[i].number8_s, mn_array[i].number8_ul, mn_array[i].number_s, mn_array[i].types);
        }
        printf("\n************************************************\n");
    }
}

/**
 * Populates and processes an array of MagicNumber structures.
 *
 * The function performs several operations on each MagicNumber element in the provided array:
 * - Truncates the `number_s` field to 8 characters (or fewer if the string length is smaller)
 * - Allocates memory for and assigns the truncated string to the `number8_s` field
 * - Converts the truncated string to an unsigned long and assigns it to the `number8_ul` field
 *
 * @param mn_array A pointer to an array of MagicNumber structures, where each element will be processed.
 */
void p_populate_struct(MagicNumber *mn_array) {
    for (int i=0; i < SIGNATURES_VECTOR_LENGTH; i++) {
        const char* n = mn_array[i].number_s;
        int size = 8;
        if (strlen(n) <size)
            size = (int) strlen(n);

        mn_array[i].number8_s = malloc((size + 1) * sizeof(char));
        strncpy(mn_array[i].number8_s, mn_array[i].number_s, size * sizeof(char) );
        mn_array[i].number8_s[size]=0;

        // Converts in unsigned long
        mn_array[i].number8_ul = strtoul(mn_array[i].number8_s, NULL, 16);
    }
}
