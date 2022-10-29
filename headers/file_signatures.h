#ifndef XDARTSCAN_FILE_SIGNATURES_H
#define XDARTSCAN_FILE_SIGNATURES_H

typedef struct MN
{
    char* number_s;
    char* types;
    char* number8_s;        // number_s truncated to 8 bytes
    unsigned long number8_ul; // number as unsigned long
} MagicNumber;

extern MagicNumber g_well_known_mn[];

#define SIGNATURES_VECTOR_LENGTH 397

#endif //XDARTSCAN_FILE_SIGNATURES_H
