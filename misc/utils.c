#include "../headers/utils.h"
#include "../headers/config.h"

#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void format_size(const unsigned long long bytes, char *result, const size_t result_size) {
    const char *units[] = {"B", "KB", "MB", "GB"};
    int unit_index = 0;
    double size = bytes;

    while (size >= 1024.0 && unit_index < 3) {
        size /= 1024.0;
        unit_index++;
    }

    if (unit_index == 0) {
        snprintf(result, result_size, "%ld %s", bytes, units[unit_index]);
    } else {
        snprintf(result, result_size, "%.2f %s", size, units[unit_index]);
    }
}

char *strnstr(const char *s, const char *find, size_t slen) {
    char c, sc;
    size_t len;

    if ((c = *find++) != '\0') {
        len = strlen(find);
        do {
            do {
                if (slen-- < 1 || (sc = *s++) == '\0')
                    return (NULL);
            } while (sc != c);
            if (len > slen)
                return (NULL);
        } while (strncmp(s, find, len) != 0);
        s--;
    }
    return ((char *) s);
}

int is_regular_file(const char *path) {
    struct stat path_stat;
    return !stat(path, &path_stat) &&
           (S_ISREG(path_stat.st_mode));
}

char *normalize_path(const char *path) {
    char *normalized_path = malloc(MAX_PATH_BUFFER * sizeof(char));
    strcpy(normalized_path, path);
    const size_t len = strlen(normalized_path);
    if (len > 0 && (normalized_path[len - 1] == '/' || normalized_path[len - 1] == '\\')) {
        normalized_path[len - 1] = '\0';
    }
    return normalized_path;
}

bool is_directory(const char *path) {
    struct stat path_stat;
    return !stat(path, &path_stat) && S_ISDIR(path_stat.st_mode);
}

/**
 * Read a binary file in memory
 *
 * @param fp
 * @param bytes_to_read
 * @return
 */
unsigned char *read_file_content(FILE *fp, unsigned long bytes_to_read) {
    unsigned char *buffer = NULL;

    if (fseek(fp, 0, SEEK_SET) != 0) {
        return NULL;
    }

    buffer = (unsigned char *) malloc(bytes_to_read * sizeof(unsigned char));
    if (buffer == NULL) {
        return NULL;
    }
    size_t bytes_read = fread(buffer, sizeof(unsigned char), bytes_to_read, fp);

    if (bytes_read != bytes_to_read) {
        free(buffer);
        return NULL;
    }

    return buffer;
}


char *trim(const char *src) {
    char *dst = malloc(sizeof(char) * (strlen(src) + 1));
    int k = 0;
    for (int j = 0; src[j] != '\0'; j++) {
        if (!(src[j] == ' ' || src[j] == '\t' || src[j] == '\n')) {
            dst[k] = src[j];
            k++;
        }
    }
    dst[k] = 0;

    return dst;
}

/**
 * Return the file length
 * @param fp
 * @return -1 for problem otherwise the length
 */
long read_file_length(FILE *fp) {
    if (fseek(fp, 0, SEEK_END) == 0)
        return ftell(fp);
    return -1;
}


// Function to swap two numbers
void swap(char *x, char *y) {
    char t = *x;
    *x = *y;
    *y = t;
}

// Function to reverse `buffer[i…j]`
char *reverse(char *buffer, int i, int j) {
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }

    return buffer;
}

char *itoa(int value, char *buffer, int base) {
    // invalid input
    if (base < 2 || base > 32) {
        return buffer;
    }

    // consider the absolute value of the number
    int n = abs(value);

    int i = 0;
    while (n) {
        int r = n % base;

        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        } else {
            buffer[i++] = 48 + r;
        }

        n = n / base;
    }

    // if the number is 0
    if (i == 0) {
        buffer[i++] = '0';
    }

    // If the base is 10 and the value is negative, the resulting string
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if (value < 0 && base == 10) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0'; // null terminate string

    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}


void make_stats(char *root_path, const double time_spent, char *buffer) {
    int offset = 0;
    int written = 0;

    // Statistics

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset,
                       "\n---------------------------- STATS ----------------------------");
    offset += (written > 0) ? written : 0;

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset,
                       "\nInput directory:                                              \"%s\"", root_path);
    offset += (written > 0) ? written : 0;

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset,
                       "\nNumber of files scanned:                                      %d", g_stats.num_files);
    offset += (written > 0) ? written : 0;

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset,
                       "\nNumber of files with High Entropy:                            %d",
                       g_stats.num_files_with_high_entropy);
    offset += (written > 0) ? written : 0;

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset,
                       "\nNumber of files with low Entropy:                             %d",
                       g_stats.num_files_with_low_entropy);
    offset += (written > 0) ? written : 0;

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset,
                       "\nNumber of files with Well Known Magic Number:                 %d",
                       g_stats.num_files_with_well_known_magic_number);
    offset += (written > 0) ? written : 0;

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset,
                       "\nNumber of files with zero size or less of magic number_s size:  %d",
                       g_stats.num_files_with_size_zero_or_less);
    offset += (written > 0) ? written : 0;

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset,
                       "\nNumber of files with length < min_size:                       %d",
                       g_stats.num_files_with_min_size);
    offset += (written > 0) ? written : 0;

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset,
                       "\nNumber of files with ERRS:                       %d", g_stats.num_files_with_errs);
    offset += (written > 0) ? written : 0;

    char tp[MAX_PATH_BUFFER];
    format_size(g_stats.size_files, tp, MAX_PATH_BUFFER);

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset,
                       "\nSize processed is %s (%llu byte)", tp, g_stats.size_files);
    offset += (written > 0) ? written : 0;

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset,
                       "\nTime elapsed is %f seconds", time_spent);
    offset += (written > 0) ? written : 0;

    format_size(g_stats.size_files / time_spent, tp, MAX_PATH_BUFFER);
    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset,
                       "\nThroughput is %s/seconds", tp);
    offset += (written > 0) ? written : 0;

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset,
                       "\n---------------------------- ***** ----------------------------");
    offset += (written > 0) ? written : 0;

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset,
                   "\n---------------------------- CONFIG ----------------------------");
    offset += (written > 0) ? written : 0;

    // Configuration

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset, "\nConfig param: %s value: \t\t%f","ENTROPY_TH",ENTROPY_TH);
    offset += (written > 0) ? written : 0;

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset, "\nConfig param: %s value: \t\t%d","DEBUG_PRINT",DEBUG_PRINT);
    offset += (written > 0) ? written : 0;

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset, "\nConfig param: %s value: \t\t%d","THROUGHPUT_TEST",THROUGHPUT_TEST);
    offset += (written > 0) ? written : 0;

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset, "\nConfig param: %s value: \t\t%d","MIN_FILE_SIZE",MIN_FILE_SIZE);
    offset += (written > 0) ? written : 0;

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset, "\nConfig param: %s value: \t\t%d","MAX_FILE_SIZE",MAX_FILE_SIZE);
    offset += (written > 0) ? written : 0;

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset,
                       "\n---------------------------- ***** ----------------------------");
    offset += (written > 0) ? written : 0;

}
