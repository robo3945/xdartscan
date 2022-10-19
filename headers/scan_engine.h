#ifndef XDARTSCAN_SCAN_ENGINE_H
#define XDARTSCAN_SCAN_ENGINE_H

void scan(char *optarg, bool verbose);
void scan_a_file(char *basePath, bool verbose);
void scanFilesRecursively(char *basePath, int root, bool verbose);
double calc_rand_idx(const unsigned char* content, long content_length, bool verbose);

#endif //XDARTSCAN_SCAN_ENGINE_H



