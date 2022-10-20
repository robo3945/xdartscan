//
// Created by roberto.battistoni on 20/10/2022.
//


#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "../headers/config.h"
#include "config_manager.h"

#define CONFIG_MAXLINE 2048
#define CONFIG_MAXPARAM 256

// Default value
double ENTROPY_TH=7.00;
int DEBUG_PRINT=1;

char* trim(const char *src)
{
    char *dst = malloc(sizeof(char)*CONFIG_MAXPARAM);
    int k=0;
    for (int j = 0; src[j] != '\0'; j++) {

        if (!(src[j] == ' ' || src[j] == '\t' || src[j] == '\n')) {
            dst[k] = src[j];
            k++;
        }
    }
    dst[k] = 0;

    return dst;
}

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
        while (!feof(fp)) {
            fgets(line, CONFIG_MAXLINE, fp);

            // TODO: finire la tokenizzazione della configurazione

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
                            if (strncmp(param_name, "ENTROPY_TH", CONFIG_MAXPARAM - 1) == 0)
                                ENTROPY_TH = strtod(param_value, NULL);
                            else if (strncmp(param_name, "DEBUG_PRINT", CONFIG_MAXPARAM - 1) == 0)
                                DEBUG_PRINT = strtol(param_value, NULL, 10);
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
    } else {
        fprintf(stderr, "Access problem to the configuration file: %s\n", filename);
        return 1;
    }

    return 0;
}