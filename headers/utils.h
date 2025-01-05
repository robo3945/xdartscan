#ifndef XDARTSCAN_UTILS_H
#define XDARTSCAN_UTILS_H
#include <stddef.h>


char* trim(const char *src);

char* itoa(int value, char* buffer, int base);

void format_size(long bytes, char *result, size_t result_size);

#endif //XDARTSCAN_UTILS_H
