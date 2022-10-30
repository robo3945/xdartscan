#ifndef XDARTSCAN_REPORT_MANAGER_H
#define XDARTSCAN_REPORT_MANAGER_H

bool create_report(char* full_path);
void close_file();
void append_to_report(char* line);

#endif //XDARTSCAN_REPORT_MANAGER_H
