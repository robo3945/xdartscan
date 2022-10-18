#ifndef XDARTSCAN_SCAN_ENGINE_H
#define XDARTSCAN_SCAN_ENGINE_H

static const int MAX_MN_FOUND_SIZE = 2048;

void scan(char *optarg, bool verbose);
void scan_a_file(char *basePath, bool verbose);
void scanFilesRecursively(char *basePath, int root, bool verbose);
double calc_rand_idx(const int* content, long content_length, bool verbose);


#endif //XDARTSCAN_SCAN_ENGINE_H



