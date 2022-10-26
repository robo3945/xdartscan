#ifndef XDARTSCAN_FILE_SIGNATURES_H
#define XDARTSCAN_FILE_SIGNATURES_H


typedef struct MN
{
    char* number;
    char* types;
    char* number8;  // number truncated to 4 bytes
} MagicNumber;

#define SIGNATURES_VECTOR_LENGTH 397

#endif //XDARTSCAN_FILE_SIGNATURES_H
