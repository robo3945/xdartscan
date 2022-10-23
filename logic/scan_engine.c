#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "../headers/scan_engine.h"
#include "../headers/file_signatures.h"
#include "../headers/config.h"
#include "../headers/utils.h"


static const int MAGIC_NUMBER_BYTE_SIZE = 4;

/**
 * Read a binary file in memory with the magic number (first 4 bytes)
 *
 * @param path
 * @param length_out
 * @param magic_number_out
 * @return
 */
unsigned char *read_file_content(char *path, long *length_out, unsigned char *magic_number_out) {

    unsigned char *buffer = NULL;
    FILE *fp = fopen(path, "rb");
    if (fp) {
        if (fseek(fp, 0, SEEK_END) == 0) {

            // Take the file length
            *length_out = ftell(fp);
            if (*length_out == -1) {
                fprintf(stderr, "File length problem: %s (%ld)\n", path, *length_out);
                fclose(fp);
                return NULL;
            }

            if (*length_out < MIN_FILE_SIZE) {
                stats.num_files_with_min_size++;
                fclose(fp);
                return NULL;
            }

            // TODO: spostare in configurazione
            if (*length_out>10000000)
                *length_out=10000000;

            // Allocates the buffer for the content
            buffer = (unsigned char *) malloc(*length_out * sizeof(unsigned char));
            if (fseek(fp, 0, SEEK_SET) != 0) {
                fprintf(stderr, "File Seek set to start problem: %s (%ld)", path, *length_out);
                fclose(fp);
                return NULL;
            }

            // Read the file content
            fread(buffer, sizeof(unsigned char), *length_out, fp);
            if (ferror(fp) != 0) {
                fprintf(stderr, "File access problem: %s (%ld)", path, *length_out);
                fclose(fp);
                return NULL;
            }

            // Set the magic number
            if (*length_out >= MAGIC_NUMBER_BYTE_SIZE) {
                memcpy(magic_number_out, buffer, MAGIC_NUMBER_BYTE_SIZE * sizeof(unsigned char));
            }

        }
        fclose(fp);

        return buffer;
    }

    printf("File access problem: %s (%ld)", path, *length_out);
    return NULL;
}

/**
 * The main scan function
 *
 * @param root_path
 * @param verbose
 */
void main_scan(char *root_path, bool verbose) {
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    printf(">>> %s", root_path);
    scan_files_recursively(root_path, 2, verbose);
    clock_gettime(CLOCK_REALTIME, &end);

    // time_spent = end - start
    double time_spent = (end.tv_sec - start.tv_sec) +
                        (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("\n\n\n");
    printf("\n---------------------------- STATS ---------------------------- ");
    printf("\nNumber of files scanned:                                      %d", stats.num_files);
    printf("\nNumber of files with High Entropy:                            %d", stats.num_files_with_high_entropy);
    printf("\nNumber of files with low Entropy:                             %d", stats.num_files_with_low_entropy);
    printf("\nNumber of files with Well Known Magic Number:                 %d", stats.num_files_with_well_known_magic_number);
    printf("\nNumber of files with zero size or less of magic number size:  %d", stats.num_files_with_size_zero_or_less);
    printf("\nNumber of files with length < min_size:                       %d", stats.num_files_with_min_size);

    printf("\nTime elpased is %f seconds", time_spent);
    printf("\n---------------------------- ***** ---------------------------- ");
}

/**
 * Scan recursively the base_path
 * @param base_path
 * @param indent
 * @param verbose
 */
void scan_files_recursively(char *base_path, int indent, bool verbose) {
    int i;
    char path[MAX_PATH_BUFFER];
    struct dirent *dp;
    DIR *dir = opendir(base_path);

    if (dir == NULL) {
        scan_a_file(base_path, verbose);
        return;
    }

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            printf("\n");
            for (i = 0; i < indent; i++)
                printf(" ");

            strcpy(path, base_path);
            strcat(path, "/");
            strcat(path, dp->d_name);

            printf("%c%c %s ", '|', '-', dp->d_name);

            scan_files_recursively(path, indent + 2, verbose);
        }
    }

    closedir(dir);
}

/**
 * Scan a file
 *
 * @param basePath
 * @param verbose
 */
void scan_a_file(char *basePath, bool verbose) {
    long file_length = -1;
    unsigned char magic_number[MAGIC_NUMBER_BYTE_SIZE];

    unsigned char *content = read_file_content(basePath, &file_length, magic_number);
    if (content != NULL) {
        bool magic_number_found = false;
        stats.num_files++;
        char magic_number_string[MAGIC_NUMBER_BYTE_SIZE * 2 + 1];

        if (file_length && file_length > MAGIC_NUMBER_BYTE_SIZE) {
            sprintf(magic_number_string, "%02x%02x%02x%02x", magic_number[0], magic_number[1], magic_number[2],
                    magic_number[3]);

            for (int j = 0; j < MAGIC_NUMBER_LENGTH; j++)
                if (strnstr(well_known_magic_number[j].number, magic_number_string, strlen(magic_number_string))) {
                    magic_number_found = true;
                    break;
                }

            if (!magic_number_found) {
                double H = calc_rand_idx(content, file_length);
                if (H > ENTROPY_TH) {
                    stats.num_files_with_high_entropy++;
                    printf("(>>> H: %f)", H);
                } else {
                    printf("(low entropy H: %f)", H);
                    stats.num_files_with_low_entropy++;
                }
            } else {
                if (verbose)
                    printf("(magic found: %s)", magic_number_string);

                stats.num_files_with_well_known_magic_number++;
            }
        } else {
            printf("(file size < %d bytes)", MAGIC_NUMBER_BYTE_SIZE);
            stats.num_files_with_size_zero_or_less++;
        }


        free(content);
    }
}
