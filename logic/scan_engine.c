#include <dirent.h>
#include <string.h>
#include <printf.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../headers/scan_engine.h"

char *read_file_content(char *path) {
    FILE *fp = fopen(path, "rb");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        long lengthOfFile = ftell(fp);

        char *buffer = malloc(lengthOfFile * sizeof(char));

        fseek(fp, 0, SEEK_SET);
        for (int i = 0; i < lengthOfFile; i++)
            buffer[i] = (char) fgetc(fp);

        fclose(fp);

        return buffer;
    }

    return NULL;
}


void scan(char *optarg, bool verbose) {
    listFilesRecursively(optarg, 2, verbose);
}

void listFilesRecursively(char *basePath, int root, bool verbose) {
    int i;
    char path[2048];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir) {
        char *content = read_file_content(basePath);
        double H = calc_rand_idx(content, verbose);
        printf("(H: %f)", H);
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
            listFilesRecursively(path, root + 2, verbose);
        }
    }

    closedir(dir);
}
