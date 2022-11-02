#ifndef XDARTSCAN_CONFIG_MANAGER_H
#define XDARTSCAN_CONFIG_MANAGER_H

#include "file_signatures.h"

int read_config_file(char* filename, bool verbose);
void sort_signatures(MagicNumber* mn_array);

#endif //XDARTSCAN_CONFIG_MANAGER_H
