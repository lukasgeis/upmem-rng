#include <stdio.h>
#include <time.h>

#include <omp.h>

#include "../../includes/sampling.c"
#include "../../includes/results.h"

#ifndef NR_TASKLETS
#define NR_TASKLETS 16
#endif

#ifndef N
#define N 1e7
#endif


static struct xs32 rngs[NR_TASKLETS];

uint32_t rng32(unsigned id) {
    return gen_xs32(&rngs[id]);
}


// clock() has problems in parallelism
// omp_get_wtime() is useful but returns a double which is inconsistent with the result type.
uint64_t measure_time() {
    return (uint64_t) (CLOCKS_PER_SEC * (double)omp_get_wtime());
} 


int main() {
    sampling_dpu_result DPU_RESULTS;

    uint32_t dummy[NR_TASKLETS];
    #pragma omp parallel for num_threads(NR_TASKLETS)
    for (unsigned int id = 0; id < NR_TASKLETS; id++) {
        unsigned seed = id + 1;

        rngs[id] = seed_xs32(seed);

        dummy[id] = 0;
        sampling_tasklet_result *result = &DPU_RESULTS.tasklet_results[id];

        result->clocks_per_sec = CLOCKS_PER_SEC;

        uint64_t num_cycles;

        num_cycles = measure_time();

        for (int i = 0; i < N; i++) {
            dummy[id] ^= i;
        }

        result->cycles_loop = measure_time() - num_cycles;
        num_cycles = measure_time();


        for (int i = 2; i < N; i++) {
            dummy[id] ^= openbsd(i, rng32, id);  
        }

        result->cycles_openbsd = measure_time() - num_cycles;
        num_cycles = measure_time();

        for (int i = 2; i < N; i++) {
            dummy[id] ^= java(i, rng32, id);  
        }

        result->cycles_java = measure_time() - num_cycles;
        num_cycles = measure_time();

        for (int i = 2; i < N; i++) {
            dummy[id] ^= lemire(i, rng32, id);  
        }

        result->cycles_lemire = measure_time() - num_cycles;
        num_cycles = measure_time();

        for (int i = 2; i < N; i++) {
            dummy[id] ^= flips(i, rng32, id);  
        }

        result->cycles_flips = measure_time() - num_cycles;
        num_cycles = measure_time();

        for (int i = 2; i < N; i++) {
            dummy[id] ^= rr(i, rng32, id);  
        }

        result->cycles_rr = measure_time() - num_cycles;
        num_cycles = measure_time();

        for (int i = 2; i < N; i++) {
            dummy[id] ^= rrb(i, rng32, id);  
        }

        result->cycles_rrb = measure_time() - num_cycles;
        num_cycles = measure_time();

        for (int i = 2; i < N; i++) {
            dummy[id] ^= rrf(i, rng32, id);  
        }

        result->cycles_rrf = measure_time() - num_cycles;

       
        // Prevent Compiler-Optimization of previous Loops
        result->dummy = dummy[id];
    }

    for (unsigned int each_tasklet = 0; each_tasklet < NR_TASKLETS; each_tasklet++) {
      sampling_tasklet_result *result = &DPU_RESULTS.tasklet_results[each_tasklet];

      printf("cpu,%u,cpu,%u,%lu,%.2e,loop\n", NR_TASKLETS, each_tasklet, result->cycles_loop, (double)result->cycles_loop / result->clocks_per_sec);
      printf("cpu,%u,cpu,%u,%lu,%.2e,openbsd\n", NR_TASKLETS, each_tasklet, result->cycles_openbsd, (double)result->cycles_openbsd / result->clocks_per_sec);
      printf("cpu,%u,cpu,%u,%lu,%.2e,java\n", NR_TASKLETS, each_tasklet, result->cycles_java, (double)result->cycles_java / result->clocks_per_sec);
      printf("cpu,%u,cpu,%u,%lu,%.2e,lemire\n", NR_TASKLETS, each_tasklet, result->cycles_lemire, (double)result->cycles_lemire / result->clocks_per_sec);
      printf("cpu,%u,cpu,%u,%lu,%.2e,flips\n", NR_TASKLETS, each_tasklet, result->cycles_flips, (double)result->cycles_flips / result->clocks_per_sec);
      printf("cpu,%u,cpu,%u,%lu,%.2e,rr\n", NR_TASKLETS, each_tasklet, result->cycles_rr, (double)result->cycles_rr / result->clocks_per_sec);
      printf("cpu,%u,cpu,%u,%lu,%.2e,rrb\n", NR_TASKLETS, each_tasklet, result->cycles_rrb, (double)result->cycles_rrb / result->clocks_per_sec);
      printf("cpu,%u,cpu,%u,%lu,%.2e,rrf\n", NR_TASKLETS, each_tasklet, result->cycles_rrf, (double)result->cycles_rrf / result->clocks_per_sec);
    }

    return 0;
}

