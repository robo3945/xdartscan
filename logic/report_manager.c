#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../headers/report_manager.h"

#include <string.h>

#include "../headers/utils.h"

void p_create_header();

static FILE **fp;
static FILE *fp_tsv;
static FILE *fp_txt;

bool create_report_file(char* full_path, int rand, char* ext, bool verbose){
    char path[MAX_PATH_BUFFER];
    snprintf(path, MAX_PATH_BUFFER, "%s%d.%s", full_path, rand, ext);

    if (strcmp(ext, "tsv") == 0)
        fp = &fp_tsv;
    else if (strcmp(ext, "txt") == 0)
        fp = &fp_txt;
    else
        return false;

    if ((*fp = fopen(path, "a")) != NULL) {
        verbose?printf("\n%s file created: %s\n\n",ext, path ):0;
        if (strcmp(ext, "tsv") == 0) p_create_header();
        return true;
    }
    return false;
}

void close_file() {
    if (fp_tsv != NULL)
        fclose(fp_tsv);
    if (fp_txt != NULL)
        fclose(fp_txt);
}

void append_to_report_tsv(char* line){
    fputs(line, fp_tsv);
}

void append_to_report_txt(char* line){
    fputs(line, fp_txt);
}

void p_create_header(){
    char buf[MAX_PATH_BUFFER];
    sprintf(buf, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
            "PATH",
            "FILE",
            "EXT",
            "Entropy",
            "Magic Number",
            "Magic Hex String",
            "Errs",
            "High Entropy",
            "Almost zero",
            "Min size",
            "Size",
            "CTime",
            "ATime",
            "MTime",
            "Description");
    append_to_report_tsv(buf);
}

