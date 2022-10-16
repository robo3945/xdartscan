//
// Created by Roberto on 15/10/22.
//

#include <dirent.h>
#include <string.h>
#include <printf.h>
#include "headers/scan_engine.h"


/**
 * Scan function
 * @param optarg
 */
void scan(char *optarg) {

    listFilesRecursively(optarg, 2);
}

void listFilesRecursively(char *basePath, int root) {
    int i;
    char path[2048];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir) {

        // TODO: to do file scan
        return;
    }

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            for (i=0; i<root; i++)
                printf(" ");
            printf("%c%c%s\n", '|', '-', dp->d_name);

            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            listFilesRecursively(path, root + 2);
        }
    }

    closedir(dir);
}
