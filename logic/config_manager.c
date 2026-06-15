#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "../headers/file_signatures.h"
#include "../headers/config_manager.h"
#include "../headers/config.h"
#include "../headers/utils.h"

#define CONFIG_MAXLINE 2048

/* Known config keys (must match the example shown by -h) */
static const char * const KNOWN_KEYS[] = {
    "ENTROPY_TH", "DEBUG_PRINT", "THROUGHPUT_TEST",
    "MIN_FILE_SIZE", "MAX_FILE_SIZE", "NUM_THREADS"
};
#define KNOWN_KEYS_COUNT 6

/**
 * Parse integer strictly: all non-whitespace chars after digits → fail.
 * Returns true on success, false on parse error.
 */
static bool parse_int_strict(const char *s, int *out) {
    if (!s || !*s) return false;
    char *end = NULL;
    long val = strtol(s, &end, 10);
    if (end == s) return false;          /* no digits consumed */
    while (*end) { if (!isspace((unsigned char)*end)) return false; end++; }
    *out = (int)val;
    return true;
}

/**
 * Parse double strictly (locale-aware, so comma decimal separator works on Windows OCP).
 * Returns true on success, false on parse error.
 */
static bool parse_double_strict(const char *s, double *out) {
    if (!s || !*s) return false;
    char *end = NULL;
    double val = strtod(s, &end);
    if (end == s) return false;
    while (*end) { if (!isspace((unsigned char)*end)) return false; end++; }
    *out = val;
    return true;
}

/**
 * Check if the key is among the 6 known config keys.
 */
static bool is_known_key(const char *key) {
    for (int i = 0; i < KNOWN_KEYS_COUNT; i++) {
        if (strcmp(key, KNOWN_KEYS[i]) == 0) return true;
    }
    return false;
}

// Default value
double ENTROPY_TH=7.00;
int DEBUG_PRINT=1;
int THROUGHPUT_TEST=0;
int MIN_FILE_SIZE=500; //bytes
int MAX_FILE_SIZE=10000000; //bytes
int NUM_THREADS=4;
static bool g_num_threads_set_from_cli = false;

// Definition for global g_stats
GlobStat g_stats = {};

void p_populate_struct(MagicNumber *mn_array);

/**
 * Reads a configuration file and parses its contents into respective global parameters.
 *
 * Validates format against the 6 known keys shown by -h: ENTROPY_TH, DEBUG_PRINT,
 * THROUGHPUT_TEST, MIN_FILE_SIZE, MAX_FILE_SIZE, NUM_THREADS.
 *
 * Validation policy:
 *   - FATAL (exit EXIT_FAILURE): non-numeric value for a known key; MIN_FILE_SIZE > MAX_FILE_SIZE.
 *   - WARNING (stderr, use default/clamp, continue): unknown key; malformed line (no '=');
 *     out-of-range value for a known key.
 *   - Silently ignored: blank lines; lines starting with '#' or ';' (comments).
 *
 * @param filename The path to the configuration file to be read.
 * @param verbose  Print parsed params to stdout when true.
 * @return 0 on success, 1 if the file could not be opened.
 */
int read_config_file(char* filename, const bool verbose) {
    FILE *fp;

    // Open and parse the INI-style config file
    if ((fp = fopen(filename, "r")) != NULL) {
        printf("\n---------------------------- CONFIG ---------------------------- \n");
        printf("Config path: %s\n\n", filename);

        bool fatal_error = false;   /* set to true on any FATAL parse error */
        int  line_no     = 0;
        char line[CONFIG_MAXLINE];

        while (fgets(line, CONFIG_MAXLINE, fp) != NULL) {
            line_no++;

            /* --- skip blank lines and comment lines (#, ;) --- */
            char *trimmed_line = trim(line);
            bool skip = (trimmed_line[0] == '\0' ||
                         trimmed_line[0] == '#'  ||
                         trimmed_line[0] == ';');
            free(trimmed_line);
            if (skip) continue;

            /* --- split on first '=' only --- */
            const char *delim = "=";
            char *token      = strtok(line, delim);
            char *param_name = NULL, *param_value = NULL;
            int   num_token  = 0;

            while (token != NULL) {
                switch (num_token) {
                    case 0:
                        param_name = trim(token);
                        break;
                    case 1:
                        if (param_name) {
                            param_value = trim(token);
                        } else {
                            fprintf(stderr, "[CONFIG] line %d: param name is NULL: %s\n", line_no, token);
                        }
                        break;
                    default:
                        /* extra '=' in line: treat everything after the first value as part of value
                           (re-join is not easy with strtok; just warn and ignore extra tokens) */
                        fprintf(stderr, "[CONFIG] line %d: extra '=' ignored: %s\n", line_no, token);
                        break;
                }
                token = strtok(NULL, delim);
                num_token++;
            }

            /* --- malformed line: no '=' found --- */
            if (param_name && param_value == NULL) {
                if (strlen(param_name) > 0) {
                    fprintf(stderr, "[CONFIG] line %d WARNING: malformed line (expected KEY=VALUE): \"%s\"\n",
                            line_no, param_name);
                }
                free(param_name);
                continue;
            }
            if (!param_name || !param_value) {
                if (param_name) free(param_name);
                if (param_value) free(param_value);
                continue;
            }

            /* --- unknown key warning --- */
            if (!is_known_key(param_name)) {
                fprintf(stderr, "[CONFIG] line %d WARNING: unknown key \"%s\" (ignored)\n",
                        line_no, param_name);
                free(param_name);
                free(param_value);
                continue;
            }

            /* --- validate and assign each known key --- */
            if (strcmp(param_name, "ENTROPY_TH") == 0) {
                double v;
                if (!parse_double_strict(param_value, &v)) {
                    fprintf(stderr, "[CONFIG] line %d ERROR: ENTROPY_TH has non-numeric value \"%s\"\n",
                            line_no, param_value);
                    fatal_error = true;
                } else {
                    if (v < 0.0 || v > 8.0) {
                        fprintf(stderr, "[CONFIG] line %d WARNING: ENTROPY_TH=%f out of range [0.0, 8.0]; value kept\n",
                                line_no, v);
                    }
                    ENTROPY_TH = v;
                    if (verbose) printf("Config param: ENTROPY_TH value: \t\t%f\n", ENTROPY_TH);
                }
            }
            else if (strcmp(param_name, "DEBUG_PRINT") == 0) {
                int v;
                if (!parse_int_strict(param_value, &v)) {
                    fprintf(stderr, "[CONFIG] line %d ERROR: DEBUG_PRINT has non-numeric value \"%s\"\n",
                            line_no, param_value);
                    fatal_error = true;
                } else {
                    if (v != 0 && v != 1) {
                        fprintf(stderr, "[CONFIG] line %d WARNING: DEBUG_PRINT=%d not in {0,1}; clamped to %d\n",
                                line_no, v, v ? 1 : 0);
                        v = v ? 1 : 0;
                    }
                    DEBUG_PRINT = v;
                    if (verbose) printf("Config param: DEBUG_PRINT value: \t\t%d\n", DEBUG_PRINT);
                }
            }
            else if (strcmp(param_name, "THROUGHPUT_TEST") == 0) {
                int v;
                if (!parse_int_strict(param_value, &v)) {
                    fprintf(stderr, "[CONFIG] line %d ERROR: THROUGHPUT_TEST has non-numeric value \"%s\"\n",
                            line_no, param_value);
                    fatal_error = true;
                } else {
                    if (v != 0 && v != 1) {
                        fprintf(stderr, "[CONFIG] line %d WARNING: THROUGHPUT_TEST=%d not in {0,1}; clamped to %d\n",
                                line_no, v, v ? 1 : 0);
                        v = v ? 1 : 0;
                    }
                    THROUGHPUT_TEST = v;
                    if (verbose) printf("Config param: THROUGHPUT_TEST value: \t\t%d\n", THROUGHPUT_TEST);
                }
            }
            else if (strcmp(param_name, "MIN_FILE_SIZE") == 0) {
                int v;
                if (!parse_int_strict(param_value, &v)) {
                    fprintf(stderr, "[CONFIG] line %d ERROR: MIN_FILE_SIZE has non-numeric value \"%s\"\n",
                            line_no, param_value);
                    fatal_error = true;
                } else {
                    if (v <= 0) {
                        fprintf(stderr, "[CONFIG] line %d WARNING: MIN_FILE_SIZE=%d <= 0; kept (will match few files)\n",
                                line_no, v);
                    }
                    MIN_FILE_SIZE = v;
                    if (verbose) printf("Config param: MIN_FILE_SIZE value: \t\t%d\n", MIN_FILE_SIZE);
                }
            }
            else if (strcmp(param_name, "MAX_FILE_SIZE") == 0) {
                int v;
                if (!parse_int_strict(param_value, &v)) {
                    fprintf(stderr, "[CONFIG] line %d ERROR: MAX_FILE_SIZE has non-numeric value \"%s\"\n",
                            line_no, param_value);
                    fatal_error = true;
                } else {
                    if (v <= 0) {
                        fprintf(stderr, "[CONFIG] line %d WARNING: MAX_FILE_SIZE=%d <= 0\n", line_no, v);
                    }
                    MAX_FILE_SIZE = v;
                    if (verbose) printf("Config param: MAX_FILE_SIZE value: \t\t%d\n", MAX_FILE_SIZE);
                }
            }
            else if (strcmp(param_name, "NUM_THREADS") == 0) {
                if (!g_num_threads_set_from_cli) {
                    int v;
                    if (!parse_int_strict(param_value, &v)) {
                        fprintf(stderr, "[CONFIG] line %d ERROR: NUM_THREADS has non-numeric value \"%s\"\n",
                                line_no, param_value);
                        fatal_error = true;
                    } else {
                        if (v < 1) {
                            fprintf(stderr, "[CONFIG] line %d WARNING: NUM_THREADS=%d < 1; clamped to 1\n", line_no, v);
                            v = 1;
                        }
                        NUM_THREADS = v;
                        if (verbose) printf("Config param: NUM_THREADS value: \t\t%d\n", NUM_THREADS);
                    }
                }
                /* else: CLI value takes precedence, silently skip */
            }

            free(param_name);
            free(param_value);
        }

        printf("---------------------------- ///// ---------------------------- \n");
        fclose(fp);

        /* --- cross-key validation --- */
        if (!fatal_error && MIN_FILE_SIZE > MAX_FILE_SIZE) {
            fprintf(stderr, "[CONFIG] ERROR: MIN_FILE_SIZE (%d) > MAX_FILE_SIZE (%d)\n",
                    MIN_FILE_SIZE, MAX_FILE_SIZE);
            fatal_error = true;
        }

        if (fatal_error) {
            fprintf(stderr, "[CONFIG] Fatal configuration error(s) found. Aborting.\n");
            exit(EXIT_FAILURE);
        }

    } else {
        return 1;
    }

    return 0;
}

/**
 * Marks NUM_THREADS as set from CLI so config parsing will skip it.
 * This ensures CLI value takes precedence over config.ini.
 */
void set_num_threads_from_cli(int value) {
    NUM_THREADS = value;
    if (NUM_THREADS < 1) NUM_THREADS = 1;
    g_num_threads_set_from_cli = true;
}

static int compare_magic_numbers(const void *a, const void *b) {
    const MagicNumber *ma = (const MagicNumber *)a;
    const MagicNumber *mb = (const MagicNumber *)b;
    if (ma->number8_ul < mb->number8_ul) return -1;
    if (ma->number8_ul > mb->number8_ul) return 1;
    return 0;
}

/**
 * Sorts an array of MagicNumber structures based on the `number8_ul` attribute in ascending order.
 *
 * @param mn_array A pointer to an array of MagicNumber structures to be sorted.
 */
void sort_signatures(MagicNumber* mn_array){

    // First of all, trim the signatures to first 4 bytes and converts in unsigned long
    p_populate_struct(mn_array);

    // Sort signatures using stdlib qsort — O(n log n) vs previous O(n^2) selection sort
    qsort(mn_array, SIGNATURES_VECTOR_LENGTH, sizeof(MagicNumber), compare_magic_numbers);

    if (DEBUG_PRINT) {
        printf("\n************************************************\n");
        printf("Signatures table print\n");
        for (int i = 0; i < SIGNATURES_VECTOR_LENGTH; ++i) {
            printf("n8_s: %s\t\t\tn8_ul: %lu\t\t-n_s: %s\t\t-types: %s\n", mn_array[i].number8_s, mn_array[i].number8_ul, mn_array[i].number_s, mn_array[i].types);
        }
        printf("\n************************************************\n");
    }
}

/**
 * Populates and processes an array of MagicNumber structures.
 *
 * The function performs several operations on each MagicNumber element in the provided array:
 * - Truncates the `number_s` field to 8 characters (or fewer if the string length is smaller)
 * - Allocates memory for and assigns the truncated string to the `number8_s` field
 * - Converts the truncated string to an unsigned long and assigns it to the `number8_ul` field
 *
 * @param mn_array A pointer to an array of MagicNumber structures, where each element will be processed.
 */
void p_populate_struct(MagicNumber *mn_array) {
    for (int i=0; i < SIGNATURES_VECTOR_LENGTH; i++) {
        const char* n = mn_array[i].number_s;
        int size = 8;
        if (strlen(n) <size)
            size = (int) strlen(n);

        mn_array[i].number8_s = malloc((size + 1) * sizeof(char));
        strncpy(mn_array[i].number8_s, mn_array[i].number_s, size * sizeof(char) );
        mn_array[i].number8_s[size]=0;

        // Converts in unsigned long
        mn_array[i].number8_ul = strtoul(mn_array[i].number8_s, NULL, 16);
    }
}
