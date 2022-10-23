#ifndef XDARTSCAN_SCAN_ENGINE_H
#define XDARTSCAN_SCAN_ENGINE_H

#include <stdbool.h>

#define MAX_PATH_BUFFER 2048

void main_scan(char *root_path, bool verbose);
void scan_a_file(char *basePath, bool verbose);
void scan_files_recursively(char *base_path, int indent, bool verbose);
double calc_rand_idx(const unsigned char* content, long content_length);

#endif //XDARTSCAN_SCAN_ENGINE_H



