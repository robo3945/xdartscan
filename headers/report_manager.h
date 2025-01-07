#ifndef XDARTSCAN_REPORT_MANAGER_H
#define XDARTSCAN_REPORT_MANAGER_H

bool create_report_file(char* full_path, int rand, char* ext, bool verbose);
void close_file();
void append_to_report_tsv(char* line);
void append_to_report_txt(char* line);

#endif //XDARTSCAN_REPORT_MANAGER_H
