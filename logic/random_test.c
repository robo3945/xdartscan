#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../headers/config.h"

/**
 * Calculates the entropy of a given content buffer using Shannon entropy formula.
 *
 * @param content Pointer to the content buffer. This must not be null.
 * @param content_length Length of the content buffer. Must be greater than 0.
 * @return The calculated entropy value as a double. Returns 0.0 if the input is invalid.
 */
double calc_rand_idx(const unsigned char *content, const unsigned long content_length) {
    if (!content || content_length <= 0) {
        return 0.0;
    }

    int bucket[MAX_SET_SIZE] = {0};

    const unsigned char *end = content + content_length;
    for (const unsigned char *p = content; p < end; p++) {
        bucket[*p]++;
    }

    const double inv_length = 1.0 / content_length;

    double H = 0.0;
    for (int i = 0; i < MAX_SET_SIZE; i++) {
        if (bucket[i]) {
            const double fract = bucket[i] * inv_length;
            H -= fract * log2(fract);
        }
    }

    if (DEBUG_PRINT) {
        printf("\n************************************************");
        printf("\nSet: ");
        for (int i = 0, j = 0; i < MAX_SET_SIZE; i++) {
            if (bucket[i] > 0) {
                printf("%d: ('%c', %d) - ", j++, i, bucket[i]);
            }
        }
        printf("\nSet_length: %lu", content_length);
        printf("\nSet with fract: ");
        for (int i = 0, j = 0; i < MAX_SET_SIZE; i++) {
            if (bucket[i] > 0) {
                const double fract = bucket[i] * inv_length;
                printf("%d: ('%c': %d, fract: %f) - ", j++, i, bucket[i], fract);
            }
        }
        printf("\nCrypto values: H: %f", H);
        printf("\n************************************************\n");
    }

    return H;
}