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
 *

    Entropy randomness test

        Binary Entropy from: http://rosettacode.org/wiki/Entropy#Python:_More_succinct_version

    For every byte we have 8 bits, so 256 different characters. Maximum entropy is for a string
    that contains an equal distribution on every character:

    H(X) = 256 * 1/256 * -log2(1/256) = 1 * log2(256) = 8

 *
 *
 */
double calc_rand_idx(const unsigned char *content, const long content_length, bool verbose) {

    // Alloca un bucket di MAX_SET_SIZE interi per contare le occorrenze di ogni carattere
    int* bucket  = calloc(MAX_SET_SIZE, sizeof(int));

    // Calcola le occorrenze di ogni carattere
    for (int i=0; i < content_length; i++) {
        // TODO: check non necessario perché gli unsigned char sono compresi nell'intervallo [0,255]
        if (content[i] >= 0 && content[i]<=MAX_SET_SIZE)
            bucket[content[i]]++;
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
