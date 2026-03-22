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
    // Early return on invalid input to avoid null pointer dereference
    if (!full_path || !ext) {
        return false;
    }
    
    char path[MAX_PATH_BUFFER];
    snprintf(path, MAX_PATH_BUFFER, "%s%d.%s", full_path, rand, ext);

    if (strcmp(ext, "tsv") == 0)
        fp = &fp_tsv;
    else if (strcmp(ext, "txt") == 0)
        fp = &fp_txt;
    else
        return false;

    if ((*fp = fopen(path, "a")) != NULL) {
        // Use 64KB fully-buffered I/O to batch writes and reduce write() syscalls
        setvbuf(*fp, NULL, _IOFBF, 64 * 1024);
        if (verbose) printf("\n%s file created: %s\n\n", ext, path);
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
    // Pre-defined constant header avoids sprintf formatting overhead on every call
    static const char header[] = "PATH\tFILE\tEXT\tEntropy\tMagic Number found\tMagic Hex String\tErrs\tHigh Entropy\tAlmost zero\tMin size\tSize\tCTime\tATime\tMTime\tDescription\n";
    append_to_report_tsv((char*)header);
}

