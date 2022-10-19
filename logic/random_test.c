#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "../headers/config.h"

/**
 * Computes the Entropy factor
 *
 * @param content
 * @param content_length
 * @param verbose
 * @return
 *
 */
double calc_rand_idx(const unsigned char *content, const long content_length) {

    // creates a bucket array to count the frequency of every char read from the file
    int* bucket  = calloc(MAX_SET_SIZE, sizeof(int));

    // computes the frequency of every character
    for (int i=0; i < content_length; i++) {
        // in the bucket at the position c[i] puts the frequency
        // implicit int cast of c[i] is the position in the bucket

        // redundancy check
        //if (content[i] >= 0 && content[i]<=MAX_SET_SIZE)
        bucket[(int)content[i]]++;
    }

    if (DEBUG_PRINT) printf("\n************************************************");
    if (DEBUG_PRINT) printf("\nSet: ");
    for (int i=0,j=0;i<MAX_SET_SIZE;i++)
        if (bucket[i] >0)
            if (DEBUG_PRINT) printf("%d: ('%c', %d) - ", j++, i, bucket[i]);

    unsigned long l = content_length;
    if (DEBUG_PRINT)
        printf("\nSet_length: %lu", l);

    if (DEBUG_PRINT) printf("\nSet with fract: ");
    double H = 0;
    for (int i = 0, j = 0; i < MAX_SET_SIZE; i++)
        if (bucket[i] > 0) {
            double fract = (double) bucket[i] / (double) l;
            H -= fract * log2(fract);
            if (DEBUG_PRINT)
                printf("%d: ('%c': %d, fract: %f) - ", j++, i, bucket[i], fract);
        }

    if (DEBUG_PRINT)
        printf("\nCrypto values: H: %f", H);

    if (DEBUG_PRINT) printf("\n************************************************\n");

    free(bucket);

    return H;
}
