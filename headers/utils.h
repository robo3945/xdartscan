#ifndef XDARTSCAN_UTILS_H
#define XDARTSCAN_UTILS_H
#include <stdbool.h>
#include <stdio.h>

#define MAX_PATH_BUFFER 2048
#define MAX_LINE_BUFFER 2048
#define MAX_STATS_BUFFER 16000

char* trim(const char *src);

char* itoa(int value, char* buffer, int base);

void format_size(unsigned long long bytes, char *result, size_t result_size);

int is_regular_file(const char *path);

unsigned char *read_file_content(FILE *fp, unsigned long bytes_to_read);

char* normalize_path(const char *path);

bool is_directory(const char *path);

void make_stats(char *root_path, double time_spent, char* buffer);

#endif //XDARTSCAN_UTILS_H
