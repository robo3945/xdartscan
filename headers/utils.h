#ifndef XDARTSCAN_UTILS_H
#define XDARTSCAN_UTILS_H
#include <stddef.h>
#include <stdio.h>


char* trim(const char *src);

char* itoa(int value, char* buffer, int base);

void format_size(unsigned long long bytes, char *result, size_t result_size);

int is_regular_file(const char *path);

unsigned char *read_file_content(FILE *fp, unsigned long bytes_to_read);

#endif //XDARTSCAN_UTILS_H
