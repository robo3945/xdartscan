#ifndef XDARTSCAN_CONFIG_H
#define XDARTSCAN_CONFIG_H

extern double ENTROPY_TH;
extern int DEBUG_PRINT;
extern int MIN_FILE_SIZE;
extern int MAX_FILE_SIZE;

typedef struct GlobalStatsStruct {
    int num_files;
    int num_files_with_high_entropy;
    int num_files_with_low_entropy;
    int num_files_with_well_known_magic_number;
    int num_files_with_size_zero_or_less;
    int num_files_with_min_size;
    int num_files_with_errs;
} GlobStat;

extern GlobStat g_stats;

#define BILLION  1000000000.0
#define MAX_SET_SIZE 256

#endif //XDARTSCAN_CONFIG_H
