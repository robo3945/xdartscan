#ifndef XDARTSCAN_SCAN_ENGINE_H
#define XDARTSCAN_SCAN_ENGINE_H

#include <stdbool.h>

void main_scan(char *optarg, bool verbose);
void scan_a_file(char *basePath, bool verbose);
void scan_files_recursively(char *basePath, int indent, bool verbose, int* num_files_processed_out);
double calc_rand_idx(const unsigned char* content, long content_length);

#endif //XDARTSCAN_SCAN_ENGINE_H



