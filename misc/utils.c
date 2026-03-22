#include "../headers/utils.h"
#include "../headers/config.h"

#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include <sys/stat.h>

/**
 * Formats a byte size into a human-readable string with appropriate units.
 *
 * @param bytes The size in bytes to format.
 * @param result A pointer to a character array where the formatted string will be stored.
 * @param result_size The size of the result array to ensure safe writing.
 */
void format_size(const unsigned long long bytes, char *result, const size_t result_size) {
    // Guard against null buffer or zero-size to prevent undefined behavior
    if (!result || result_size == 0) {
        return;
    }

    // Static array: initialized once at program start, avoids stack re-initialization per call
    static const char *units[] = {"B", "KB", "MB", "GB"};
    int unit_index = 0;
    double size = bytes;

    // Divide by 1024 iteratively to find the appropriate unit (B -> KB -> MB -> GB)
    while (size >= 1024.0 && unit_index < 3) {
        size /= 1024.0;
        unit_index++;
    }

    if (unit_index == 0) {
        snprintf(result, result_size, "%llu %s", bytes, units[unit_index]);
    } else {
        snprintf(result, result_size, "%.2f %s", size, units[unit_index]);
    }
}


/**
 * Formats time from seconds (with milliseconds) to HH:MM:SS.mmm format
 *
 * @param seconds Total number of seconds with milliseconds as double
 * @param buffer Buffer where the formatted string will be written
 * @param buffer_size Size of the buffer
 * @return 0 if successful, -1 if error
 */
int format_time(const double seconds, char* const buffer, const size_t buffer_size) {
    if (buffer == NULL || buffer_size < 13) {  // HH:MM:SS.mmm\0 needs 13 characters
        return -1;  // Invalid or too small buffer
    }

    // Handle negative numbers
    if (seconds < 0) {
        return -1;
    }

    // Extract the integer part of seconds
    const unsigned long whole_seconds_total = (unsigned long)floor(seconds);

    // Calculate milliseconds (decimal part)
    const unsigned int milliseconds = (unsigned int)((seconds - whole_seconds_total) * 1000);

    // Calculate hours, minutes and seconds
    const unsigned int hours = whole_seconds_total / 3600;
    const unsigned int remaining_after_hours = whole_seconds_total % 3600;
    const unsigned int minutes = remaining_after_hours / 60;
    const unsigned int seconds_final = remaining_after_hours % 60;

    // Format the string in the buffer
    const int written = snprintf(buffer, buffer_size, "%02u:%02u:%02u.%03u",
                          hours, minutes, seconds_final, milliseconds);

    if (written < 0 || written >= buffer_size) {
        return -1;  // Formatting error
    }

    return 0;
}

char *strnstr(const char *s, const char *find, size_t slen) {
    // Empty needle always matches at the start of the haystack
    if (*find == '\0')
        return (char *)s;

    char c, sc;
    size_t len;

    c = *find++;
    len = strlen(find);

    // Early exit: needle longer than haystack can never match
    if (len >= slen)
        return NULL;
        
    do {
        do {
            if (slen-- < 1 || (sc = *s++) == '\0')
                return NULL;
        } while (sc != c);
        if (len > slen)
            return NULL;
    } while (strncmp(s, find, len) != 0);
    
    return (char *)(s - 1);
}

/**
 * Determines if a given file path corresponds to a regular file.
 *
 * @param path The file path to check.
 * @return Non-zero if the path corresponds to a regular file; otherwise, 0.
 */
int is_regular_file(const char *path) {
    struct stat path_stat;
    return !stat(path, &path_stat) &&
           (S_ISREG(path_stat.st_mode));
}

/**
 * Normalize a file system path by removing a trailing slash or backslash if present.
 *
 * @param path The input path string to normalize.
 * @return A dynamically allocated string containing the normalized path.
 *         It is the caller's responsibility to free the allocated memory.
 */
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
 * Reads the content of a file into a dynamically allocated buffer.
 *
 * @param fp A pointer to an open file stream from which the content will be read.
 * @param bytes_to_read The number of bytes to read from the file.
 * @return A pointer to the dynamically allocated buffer containing the file content,
 *         or NULL if an error occurs (e.g., file read error, memory allocation failure).
 */
unsigned char *read_file_content(FILE *fp, unsigned long bytes_to_read) {
    // Guard: skip allocation and I/O for invalid inputs
    if (!fp || bytes_to_read == 0) {
        return NULL;
    }

    if (fseek(fp, 0, SEEK_SET) != 0) {
        return NULL;
    }

    // Use malloc instead of calloc: the buffer is immediately overwritten by fread,
    // so zero-initialization would be wasted work
    unsigned char *buffer = (unsigned char *) malloc(bytes_to_read);
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

/**
 * Converts an integer value to a null-terminated string using the specified base.
 *
 * @param value The integer value to convert.
 * @param buffer Pointer to the buffer where the resulting string will be stored.
 *               The buffer must be large enough to contain the resulting string,
 *               including the null-terminating character.
 * @param base The numerical base to use for the conversion. Supported bases are
 *             between 2 and 32, inclusive. If the base is 10 and the value is
 *             negative, the string will be prefixed with a minus sign.
 * @return A pointer to the buffer containing the resulting string representation.
 */
char *itoa(int value, char *buffer, int base) {
    // Guard against unsupported bases and null buffer
    if (base < 2 || base > 32 || buffer == NULL) {
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

    char size_formatted[MAX_PATH_BUFFER];
    format_size(g_stats.size_files, size_formatted, MAX_PATH_BUFFER);

    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset,
                       "\nSize processed is %s (%llu byte)", size_formatted, g_stats.size_files);
    offset += (written > 0) ? written : 0;


    char time_formatted[13];
    const int ret = format_time(time_spent, time_formatted, sizeof(time_formatted));

    if (!ret) {
        written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset,
                           "\nTime elapsed is: %s (%f secs)", time_formatted, time_spent);
    } else {
        written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset,
                       "\nTime elapsed is: %f seconds", time_spent);
    }
    offset += (written > 0) ? written : 0;

    format_size(g_stats.size_files / time_spent, size_formatted, MAX_PATH_BUFFER);
    written = snprintf(buffer + offset, MAX_LINE_BUFFER - offset,
                       "\nThroughput is %s/seconds", size_formatted);
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
