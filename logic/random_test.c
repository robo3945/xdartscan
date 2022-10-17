#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

static const int MAX_SET_SIZE = 256;

double calc_rand_idx(const int *content, const long file_length, bool verbose) {
/*
Entropy randomness test

        Binary Entropy from: http://rosettacode.org/wiki/Entropy#Python:_More_succinct_version

For every byte we have 8 bits, so 256 different characters. Maximum entropy is for a string
that contains an equal distribution on every character:

H(X) = 256 * 1/256 * -log2(1/256) = 1 * log2(256) = 8

:param content:
:param verbose:
:return:
*/

    // Alloca un bucket di MAX_SET_SIZE interi per contare le occorrenze di ogni carattere
    int* bucket  = calloc(MAX_SET_SIZE, sizeof(int));

    // Calcola le occorrenze di ogni carattere
    for (int i=0;i<file_length;i++) {
        if (content[i] > 0 && content[i]<=MAX_SET_SIZE)
            bucket[content[i]]++;
    }

    if (verbose) printf("\n************************************************");
    if (verbose) printf("\nSet: ");
    for (int i=0,j=0;i<MAX_SET_SIZE;i++)
        if (bucket[i] >0)
            if (verbose) printf("%d: ('%c', %d) - ", j++, i, bucket[i]);

    unsigned long l = file_length;
    if (verbose)
        printf("\nSet_length: %lu", l);

    if (verbose) printf("\nSet with fract: ");
    double H = 0;
    for (int i = 0, j = 0; i < MAX_SET_SIZE; i++)
        if (bucket[i] > 0) {
            double fract = (double) bucket[i] / (double) l;
            H -= fract * log2(fract);
            if (verbose)
                printf("%d: ('%c': %d, fract: %f) - ", j++, i, bucket[i], fract);
        }

    if (verbose)
        printf("\nCrypto values: H: %f", H);

    if (verbose) printf("\n************************************************\n");

    free(bucket);

    return H;
}
