#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "../headers/file_signatures.h"
#include "../headers/config_manager.h"
#include "../headers/config.h"
#include "../headers/utils.h"

#define CONFIG_MAXLINE 2048
#define CONFIG_MAXPARAM 256

// Default value
double ENTROPY_TH=7.00;
int DEBUG_PRINT=1;
int MIN_FILE_SIZE=500; //bytes
int MAX_FILE_SIZE=10000000; //bytes

// Definition for global stats
GlobStat stats = {};

void populate_struct(MagicNumber *mn_array);

/**
 * read the config file
 *
 * @param filename
 * @return 0 if file was read 1 otherwise
 */
int read_config_file(char* filename) {
    FILE *fp;
    char line[CONFIG_MAXLINE];

    if ((fp = fopen(filename, "r")) != NULL) {
        printf("\n---------------------------- CONFIG ---------------------------- \n");
        while (!feof(fp)) {
            fgets(line, CONFIG_MAXLINE, fp);

            char *token;
            char* delim = "=";
            int num_token = 0;
            token = strtok(line, delim);
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
                            if (strncmp(param_name, "ENTROPY_TH", CONFIG_MAXPARAM - 1) == 0) {
                                ENTROPY_TH = strtod(param_value, NULL);
                                printf("Config param: %s value: %f\n","ENTROPY_TH",ENTROPY_TH);
                            }
                            else if (strncmp(param_name, "DEBUG_PRINT", CONFIG_MAXPARAM - 1) == 0) {
                                DEBUG_PRINT = strtol(param_value, NULL, 10);
                                printf("Config param: %s value: %d\n","DEBUG_PRINT",DEBUG_PRINT);
                            }
                            else if (strncmp(param_name, "MIN_FILE_SIZE", MIN_FILE_SIZE - 1) == 0) {
                                MIN_FILE_SIZE = strtol(param_value, NULL, 10);
                                printf("Config param: %s value: %d\n","MIN_FILE_SIZE",MIN_FILE_SIZE);
                            }
                            else if (strncmp(param_name, "MAX_FILE_SIZE", MAX_FILE_SIZE - 1) == 0) {
                                MAX_FILE_SIZE = strtol(param_value, NULL, 10);
                                printf("Config param: %s value: %d\n","MAX_FILE_SIZE",MAX_FILE_SIZE);
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
        printf("Configuration file correctly read, path: \"%s\"\n", filename);
        printf("\n---------------------------- ///// ---------------------------- \n");

    } else
        return 1;

    return 0;
}

/**
 * Function to sort signatures
 *
 * trim the signatures to the first 4 bytes and then sort the array
 *
 */
void sort_signatures(MagicNumber* mn_array){

    // First of all, trim the signatures to first 4 bytes and converts in unsigned long
    populate_struct(mn_array);

    int min;
    for(int i=0; i<SIGNATURES_VECTOR_LENGTH; i++){
        min=i;
        for (int j = i+1;j < SIGNATURES_VECTOR_LENGTH; j++){
            if (mn_array[j].number8_ul < mn_array[min].number8_ul)
                min=j;
        }

        // Swap the objects
        MagicNumber t = mn_array[min];
        mn_array[min] = mn_array[i];
        mn_array[i] = t;
    }

    if (DEBUG_PRINT) {
        printf("\n************************************************\n");
        printf("Signatures table print\n");
        for (int i = 0; i < SIGNATURES_VECTOR_LENGTH; ++i) {
            printf("n8_s: %s\t\t\tn8_ul: %lu\t\t-n_s: %s\t\t-types: qwwqqq%s\n", mn_array[i].number8_s, mn_array[i].number8_ul, mn_array[i].number_s, mn_array[i].types);
        }
        printf("\n************************************************\n");
    }
}

/**
 * Populates the struct with other attrs
 *
 * @param mn_array
 */
void populate_struct(MagicNumber *mn_array) {
    for (int i=0; i < SIGNATURES_VECTOR_LENGTH; i++) {
        char* n = mn_array[i].number_s;
        int size = 8;
        if (strlen(n) <size)
            size = strlen(n);

        mn_array[i].number8_s = malloc((size + 1) * sizeof(char));
        strncpy(mn_array[i].number8_s, mn_array[i].number_s, size * sizeof(char) );
        mn_array[i].number8_s[size]=0;

        // Converts in unsigned long
        mn_array[i].number8_ul = strtoul(mn_array[i].number8_s, NULL, 16);
    }
}
