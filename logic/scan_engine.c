#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "../headers/scan_engine.h"
#include "../headers/file_signatures.h"

#define BILLION  1000000000.0

int *read_file_content(char *path, long* length_out, int* magic_number_out) {
    FILE *fp = fopen(path, "rb");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        *length_out = ftell(fp);

        int *buffer = malloc(*length_out * sizeof(int));

        fseek(fp, 0, SEEK_SET);
        for (int i = 0; i < *length_out; i++) {
            buffer[i] = fgetc(fp);
            if (i < 4)
                magic_number_out[i] = buffer[i];
        }

        fclose(fp);

        return buffer;
    }

    printf("File access problem: %s (%ld)", path, *length_out);
    return NULL;
}


void scan(char *optarg, bool verbose) {
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    scanFilesRecursively(optarg, 2, verbose);
    clock_gettime(CLOCK_REALTIME, &end);

    // time_spent = end - start
    double time_spent = (end.tv_sec - start.tv_sec) +
                        (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("\nTime elpased is %f seconds", time_spent);
}

/**
 * Scan all files
 * @param basePath
 * @param root
 * @param verbose
 */
void scanFilesRecursively(char *basePath, int root, bool verbose) {
    int i;
    char path[2048];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (dir==NULL) {
        scan_a_file(basePath, verbose);
        return;
    }

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            printf("\n");
            for (i = 0; i < root; i++)
                printf(" ");
            printf("%c%c%s ", '|', '-', dp->d_name);

            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            scanFilesRecursively(path, root + 2, verbose);
        }
    }

    closedir(dir);
}

/**
 * Single file scan
 *
 * @param basePath
 * @param verbose
 */
void scan_a_file(char *basePath, bool verbose) {
    long file_lenght = 0;
    int magic_number[4];

    int *content = read_file_content(basePath, &file_lenght, magic_number);
    if (content != NULL) {
        bool mg_found = false;

        char magic_number_string[8*sizeof(int)];
        sprintf(magic_number_string, "%02x%02x%02x%02x",magic_number[0],magic_number[1], magic_number[2], magic_number[3]);


        for (int j=0;j<MAGIC_NUMBER_LENGTH;j++)
            if (strstr(well_known_magic_number[j].number, magic_number_string)) {
                mg_found = true;
                break;
            }

        if (!mg_found) {
            double H = calc_rand_idx(content, file_lenght, verbose);
            if (H>7.90 && verbose)
                printf("(H: %f, magic#: %s)", H,magic_number_string);
        }
        else if (verbose)
            printf("(magic found! (%s))", magic_number_string);

        free(content);
    }
}
