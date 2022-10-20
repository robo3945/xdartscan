#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "../headers/scan_engine.h"
#include "../headers/file_signatures.h"
#include "../headers/config.h"


/**
 * Read a binary file in memory with the magic number (first 4 bytes)
 *
 * @param path
 * @param length_out
 * @param magic_number_out
 * @return
 */
unsigned char *read_file_content(char *path, long* length_out, unsigned char* magic_number_out) {

    unsigned char *buffer = NULL;
    FILE *fp = fopen(path, "rb");
    if (fp) {
        if (fseek(fp, 0, SEEK_END)==0) {

            // Take the file length
            *length_out = ftell(fp);
            if (*length_out == -1)
            {
                fprintf(stderr, "File length problem: %s (%ld)", path, *length_out);
                return NULL;
            }

            // Allocates the buffer for the content
            buffer = (unsigned char*) malloc(*length_out * sizeof(unsigned char));
            if (fseek(fp, 0, SEEK_SET)!=0)
            {
                fprintf(stderr, "File Seek set to start problem: %s (%ld)", path, *length_out);
                return NULL;
            }

            // Read the file content
            fread(buffer, sizeof(unsigned char), *length_out, fp);
            if ( ferror( fp ) != 0 ) {
                fprintf(stderr, "File access problem: %s (%ld)", path, *length_out);
                return NULL;
            }

            // Set the magic number
            memcpy(magic_number_out, buffer, 4 * sizeof(unsigned char));

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
 * @param optarg
 * @param verbose
 */
void main_scan(char *optarg, bool verbose) {
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int files_processed = 0;
    scan_files_recursively(optarg, 2, verbose, &files_processed);
    clock_gettime(CLOCK_REALTIME, &end);

    // time_spent = end - start
    double time_spent = (end.tv_sec - start.tv_sec) +
                        (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("\nNumber of files processed: %d", files_processed);
    printf("\nTime elpased is %f seconds", time_spent);
}

/**
 * Scan recursively the basePath
 * @param basePath
 * @param indent
 * @param verbose
 */
void scan_files_recursively(char *basePath, int indent, bool verbose, int* num_files_processed_out) {
    int i;
    char path[2048];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (dir==NULL) {
        scan_a_file(basePath, verbose);
        (*num_files_processed_out)++;
        return;
    }

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            printf("\n");
            for (i = 0; i < indent; i++)
                printf(" ");
            printf("%c%c%s ", '|', '-', dp->d_name);

            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            scan_files_recursively(path, indent + 2, verbose, num_files_processed_out);
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
    long file_length = 0;
    unsigned char magic_number[4];

    unsigned char *content = read_file_content(basePath, &file_length, magic_number);
    if (content != NULL) {
        bool mg_found = false;

        char magic_number_string[9];
        sprintf(magic_number_string, "%02x%02x%02x%02x",magic_number[0],magic_number[1], magic_number[2], magic_number[3]);

        for (int j=0;j<MAGIC_NUMBER_LENGTH;j++)
            if (strstr(well_known_magic_number[j].number, magic_number_string)) {
                mg_found = true;
                break;
            }

        if (!mg_found) {
            double H = calc_rand_idx(content, file_length);
            if (H > ENTROPY_TH)
                printf("(H: %f, magic#: %s)", H,magic_number_string);
            else
                printf("(low entropy)");
        }
        else if (verbose)
            printf("(magic found! (%s))", magic_number_string);

        free(content);
    }
}
