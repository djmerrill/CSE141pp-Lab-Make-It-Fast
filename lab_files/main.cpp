#include "archlab.hpp"

#include <stdlib.h>
#include <getopt.h>

#include "lab.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>


extern "C" {
    double sqmm(double * D, double * A, double * B, uint64_t size);
    double reference(double * D, double * A, double * B, uint64_t size);
}

/*
void reference (double * d, double * a, double * b, uint64_t size) {
    for (uint64_t r = 0; r < size; r += 1) {
        for (uint64_t c = 0; c < size; c += 1) {
            for (uint64_t i = 0; i < size; i += 1) {
                d[r + size * c] += a[r + size * i] * b[i + size * c];
            }
        }
    }
}
*/

int main(int argc, char * argv[]) {
    fprintf(stderr, "starting\n");

    uint64_t mat_size_small;
    uint64_t mat_size_large;
    uint32_t iterations;

    archlab_add_si_option < uint64_t > ("mat-small", mat_size_small, 512, "Small memory region size (bytes).");
    archlab_add_si_option < uint64_t > ("mat-large", mat_size_large, 1 * KB, "Large region size (bytes).");
    archlab_add_si_option < uint32_t > ("iterations", iterations, 2, "Repetitions");

    fprintf(stderr, "parsing command line\n");

    archlab_parse_cmd_line( & argc, argv);

    fprintf(stderr, "allocating\n");
    double * A = (double * ) malloc(mat_size_large * mat_size_large * sizeof(double)); // allocate two big matrices
    double * B = (double * ) malloc(mat_size_large * mat_size_large * sizeof(double));
    double * C = (double * ) malloc(mat_size_large * mat_size_large * sizeof(double)); // correct matrix
    double * D = (double * ) malloc(mat_size_large * mat_size_large * sizeof(double)); // result matrix

    fprintf(stderr, "filling\n");
    for (uint32_t i = 0; i < mat_size_large * mat_size_large; i++) {
        A[i] = rand_double(); // fill them with random data
        B[i] = rand_double();
        C[i] = 0.0;
        D[i] = 0.0;
    }

    fprintf(stderr, "testing against reference.\n");
    for (uint64_t size = 16; size < 64; size += 3) {
	fprintf(stderr, "size=%ld\n", size);
        reference(C, A, B, size);
        sqmm(D, A, B, size);
        for (uint64_t i = 0; i < mat_size_large * mat_size_large; i += 1) {
            if (abs(C[i] - D[i]) > 1e-6) {
                if (i >= size) {
                    fprintf(stderr, "Bad result from sqmm. C[%ld]=%f D[%ld]=%f. Outside bounds (size=%ld)\n", i, C[i], i, D[i], size);
		    return 0;
                } else {
                    fprintf(stderr, "Bad result from sqmm. C[%ld]=%f D[%ld]=%f.\n", i, C[i], i, D[i]);
                    return 0;
                }
            }
        }
    }
    fprintf(stderr, "passed testing\n");
    //pristine_machine(); // clear caches, disable turbo boost, reset clock speed
    if (std::getenv("MHZ") != nullptr) {
        int MHZ = atoi(std::getenv("MHZ"));
        fprintf(stderr, "MHz set to: %d\n", MHZ);
        set_cpu_clock_frequency(MHZ);
    } 

    // submission
    for (uint64_t size = mat_size_large; size >= mat_size_small; size /= 2) { // and for different vector sizes
    	for (uint64_t i = 0; i < size; i += 1) D[i] = 0.0;
        fprintf(stderr, "starting submitted code with size: %ld\n", size);
        ArchLabTimer timer;
        timer.attr("matrix_size", size).
            attr("iterations", iterations).
            attr("reference", 0).
            go(); // Start measuring
        for (uint32_t k = 0; k < iterations; k++) {
            sqmm(D, A, B, size); // Call submitted code
        }
    }

    // reference
    for (uint64_t size = mat_size_large; size >= mat_size_small; size /= 2) { // and for different vector sizes
    	for (uint64_t i = 0; i < size; i += 1) D[i] = 0.0;
        fprintf(stderr, "starting reference code with size: %ld\n", size);
        ArchLabTimer timer;
        timer.attr("matrix_size", size).
            attr("iterations", iterations).
            attr("reference", 1).
            go(); // Start measuring
        for (uint32_t k = 0; k < iterations; k++) {
            reference(D, A, B, size); // Call submitted code
        }
    }


    archlab_write_stats();

    return 0;
}
