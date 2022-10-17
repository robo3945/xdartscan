//
// Created by Roberto on 15/10/22.
//

#ifndef XDARTSCAN_SCAN_ENGINE_H
#define XDARTSCAN_SCAN_ENGINE_H

#endif //XDARTSCAN_SCAN_ENGINE_H

void scan(char *optarg, bool verbose);
void scanFilesRecursively(char *basePath, int root, bool verbose);
double calc_rand_idx(const int* content, const long content_length, bool verbose);

