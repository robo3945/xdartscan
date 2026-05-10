#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../headers/report_manager.h"
#include "../headers/utils.h"

void p_create_header(void);
static void json_escape_string(const char *src, char *dst, size_t dst_size);

static FILE **fp;
static FILE *fp_tsv;
static FILE *fp_txt;
static FILE *fp_json = NULL;

typedef struct SuspectEntry {
    char           path[MAX_PATH_BUFFER];
    double         entropy;
    char           magic_hex[9];
    unsigned long  size;
    struct SuspectEntry *next;
} SuspectEntry;

static SuspectEntry *g_suspect_head = NULL;
static SuspectEntry *g_suspect_tail = NULL;

bool create_report_file(char* full_path, int rand, char* ext, bool verbose){
    if (!full_path || !ext) {
        return false;
    }

    char path[MAX_PATH_BUFFER];
    snprintf(path, MAX_PATH_BUFFER, "%s%d.%s", full_path, rand, ext);

    if (strcmp(ext, "tsv") == 0)
        fp = &fp_tsv;
    else if (strcmp(ext, "txt") == 0)
        fp = &fp_txt;
    else
        return false;

    if ((*fp = fopen(path, "a")) != NULL) {
        // Use 64KB fully-buffered I/O to batch writes and reduce write() syscalls
        setvbuf(*fp, NULL, _IOFBF, 64 * 1024);
        if (verbose) printf("\n%s file created: %s\n\n", ext, path);
        if (strcmp(ext, "tsv") == 0) p_create_header();
        return true;
    }
    return false;
}

bool open_json_report(const char *filepath, bool verbose) {
    if (!filepath) return false;
    fp_json = fopen(filepath, "w");
    if (fp_json == NULL) return false;
    if (verbose) printf("\njson file created: %s\n\n", filepath);
    return true;
}

void close_file(void) {
    if (fp_tsv != NULL)
        fclose(fp_tsv);
    if (fp_txt != NULL)
        fclose(fp_txt);
    if (fp_json != NULL)
        fclose(fp_json);

    SuspectEntry *e = g_suspect_head;
    while (e) {
        SuspectEntry *next = e->next;
        free(e);
        e = next;
    }
    g_suspect_head = g_suspect_tail = NULL;
}

void append_to_report_tsv(char* line){
    fputs(line, fp_tsv);
}

void append_to_report_txt(char* line){
    fputs(line, fp_txt);
}

void append_suspect_to_json_list(const char *path, double entropy,
                                  const char *magic_hex, unsigned long size) {
    SuspectEntry *e = malloc(sizeof(SuspectEntry));
    if (!e) return;
    strncpy(e->path, path, MAX_PATH_BUFFER - 1);
    e->path[MAX_PATH_BUFFER - 1] = '\0';
    e->entropy = entropy;
    strncpy(e->magic_hex, magic_hex ? magic_hex : "", 8);
    e->magic_hex[8] = '\0';
    e->size = size;
    e->next = NULL;
    if (!g_suspect_head) g_suspect_head = e;
    else                 g_suspect_tail->next = e;
    g_suspect_tail = e;
}

void write_json_report(const char *scanned_dir, time_t scan_ts,
                        int total_files, int suspect_files) {
    if (!fp_json) return;

    char ts_buf[32];
    struct tm *gmt = gmtime(&scan_ts);
    strftime(ts_buf, sizeof(ts_buf), "%Y-%m-%dT%H:%M:%SZ", gmt);

    char esc[MAX_PATH_BUFFER * 2];
    json_escape_string(scanned_dir, esc, sizeof(esc));

    fprintf(fp_json, "{\n");
    fprintf(fp_json, "  \"scan_timestamp\": \"%s\",\n", ts_buf);
    fprintf(fp_json, "  \"scanned_directory\": \"%s\",\n", esc);
    fprintf(fp_json, "  \"total_files\": %d,\n", total_files);
    fprintf(fp_json, "  \"suspect_files\": %d,\n", suspect_files);
    fprintf(fp_json, "  \"suspects\": [\n");

    SuspectEntry *e = g_suspect_head;
    while (e) {
        json_escape_string(e->path, esc, sizeof(esc));
        fprintf(fp_json, "    {\"path\": \"%s\", \"entropy\": %.6f, \"magic_hex\": \"%s\", \"size\": %lu}%s\n",
                esc, e->entropy, e->magic_hex, e->size,
                e->next ? "," : "");
        e = e->next;
    }

    fprintf(fp_json, "  ]\n}\n");
    fflush(fp_json);
}

void p_create_header(void){
    static const char header[] = "PATH\tFILE\tEXT\tSuspect\tEntropy\tMagic Number found\tMagic Hex String\tErrs\tHigh Entropy\tAlmost zero\tMin size\tSize\tCTime\tATime\tMTime\tDescription\n";
    append_to_report_tsv((char*)header);
}

static void json_escape_string(const char *src, char *dst, size_t dst_size) {
    size_t j = 0;
    for (size_t i = 0; src[i] && j + 2 < dst_size; i++) {
        if (src[i] == '"' || src[i] == '\\') {
            dst[j++] = '\\';
        }
        dst[j++] = src[i];
    }
    dst[j] = '\0';
}
