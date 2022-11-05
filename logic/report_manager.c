#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/report_manager.h"
#include "../headers/scan_engine.h"

void p_create_header();

static FILE *fp = NULL;

bool create_report(char* full_path, bool verbose){
    char path[MAX_PATH_BUFFER];
    sprintf(path, "%s%d.tsv", full_path, rand());

    if ((fp = fopen(path, "a")) != NULL) {
        (verbose)?printf("\nTSV file created: %s\n\n",path ):0;
        p_create_header();
        return true;
    }
    return false;
}

void append_to_report(char* line){
    fputs(line, fp);
}

void p_create_header(){
    char buf[MAX_PATH_BUFFER];
    sprintf(buf, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", "PATH",
            "Entropy",
            "Magic Number",
            "High Entropy",
            "Almost zero",
            "Min size",
            "Size",
            "CTime",
            "ATime",
            "MTime");
    append_to_report(buf);
}

void close_file() {
    if (fp != NULL)
        fclose(fp);
}

