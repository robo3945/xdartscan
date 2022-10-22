#ifndef XDARTSCAN_CONFIG_H
#define XDARTSCAN_CONFIG_H

extern double ENTROPY_TH;
extern int DEBUG_PRINT;
typedef struct GlobalStatsStruct {
    int num_files;
    int num_files_with_high_entropy;
    int num_files_with_low_entropy;
    int num_files_with_well_known_magic_number;
} GlobStat;

extern GlobStat statistics;

#define BILLION  1000000000.0
#define MAX_SET_SIZE 256

#endif //XDARTSCAN_CONFIG_H
