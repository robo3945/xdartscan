#ifndef XDARTSCAN_CONFIG_H
#define XDARTSCAN_CONFIG_H

// TODO: move the config params into an external file
extern double ENTROPY_TH;
extern int DEBUG_PRINT;

#define BILLION  1000000000.0
#define MAX_SET_SIZE 256

void init_config_variable();

#endif //XDARTSCAN_CONFIG_H
