#include <stdio.h>
#include <stdlib.h>
#include <archlab.h>


void reference (double * d, double * a, double * b, uint64_t size) {
    for (uint64_t r = 0; r < size; r += 1) {
        for (uint64_t c = 0; c < size; c += 1) {
            for (uint64_t i = 0; i < size; i += 1) {
                d[r + size * c] += a[r + size * i] * b[i + size * c];
            }
        }
    }
}
