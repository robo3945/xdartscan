#ifndef XDARTSCAN_REPORT_MANAGER_H
#define XDARTSCAN_REPORT_MANAGER_H

#include <stdbool.h>
#include <time.h>

bool create_report_file(char* full_path, int rand, char* ext, bool verbose);
bool open_json_report(const char *filepath, bool verbose);
void close_file(void);
void append_to_report_tsv(char* line);
void append_to_report_txt(char* line);
void append_suspect_to_json_list(const char *path, double entropy,
                                  const char *magic_hex, unsigned long size);
void write_json_report(const char *scanned_dir, time_t scan_ts,
                        int total_files, int suspect_files);

#endif //XDARTSCAN_REPORT_MANAGER_H
