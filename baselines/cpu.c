#include <stdio.h>
#include <time.h>

#include <omp.h>

#include "../includes/rng.c"
#include "../includes/results.h"

int main() {
    dpu_results_t DPU_RESULTS;

    uint32_t dummy[NR_TASKLETS];

    #pragma omp parallel for
    for (unsigned int id = 0; id < NR_TASKLETS; id++) {
        dummy[id] = 0;
        dpu_tasklet_result_t *result = &DPU_RESULTS.tasklet_results[id];

        uint64_t num_cycles;

        num_cycles = clock();

        for (int i = 0; i < N; i++) {
            dummy[id] ^= i;
        }

        result->cycles_null = clock() - num_cycles;
        num_cycles = clock();


        struct xs32 rng_xs32 = seed_xs32(id);

        for (int i = 0; i < N; i++) {
            dummy[id] ^= gen_xs32(&rng_xs32);    
        }

        result->cycles_xs32 = clock() - num_cycles;
        num_cycles = clock();

        struct mt32 rng_mt32 = seed_mt32(id);

        for (int i = 0; i < N; i++) {
            dummy[id] ^= gen_mt32(&rng_mt32);    
        }

        result->cycles_mt32 = clock() - num_cycles;
        num_cycles = clock();

        struct sc32 rng_sc32 = seed_sc32(id);

        for (int i = 0; i < N; i++) {
            dummy[id] ^= gen_sc32(&rng_sc32);    
        }

        result->cycles_sc32 = clock() - num_cycles;
        num_cycles = clock();

        struct lm32 rng_lm32 = seed_lm32(id);

        for (int i = 0; i < N; i++) {
            dummy[id] ^= gen_lm32(&rng_lm32);    
        }

        result->cycles_lm32 = clock() - num_cycles;
        num_cycles = clock();

        struct pcg32 rng_pcg32 = seed_pcg32(id);

        for (int i = 0; i < N; i++) {
            dummy[id] ^= gen_pcg32(&rng_pcg32);    
        }

        result->cycles_pcg32 = clock() - num_cycles;
        num_cycles = clock();

        struct cha32 rng_cha32 = seed_cha32(id);

        for (int i = 0; i < N; i++) {
            dummy[id] ^= gen_cha32(&rng_cha32);    
        }

        result->cycles_cha32 = clock() - num_cycles;

        result->clocks_per_sec = CLOCKS_PER_SEC;

        // Prevent Compiler-Optimization of previous Loops
        result->dummy = dummy[id];
    }

    for (unsigned int each_tasklet = 0; each_tasklet < NR_TASKLETS; each_tasklet++) {
      dpu_tasklet_result_t *result = &DPU_RESULTS.tasklet_results[each_tasklet];

      printf("cpu,%u,cpu,%u,%lu,%.2e,null\n", NR_TASKLETS, each_tasklet, result->cycles_null, (double)result->cycles_null / result->clocks_per_sec);
      printf("cpu,%u,cpu,%u,%lu,%.2e,xs32\n", NR_TASKLETS, each_tasklet, result->cycles_xs32, (double)result->cycles_xs32 / result->clocks_per_sec);
      printf("cpu,%u,cpu,%u,%lu,%.2e,mt32\n", NR_TASKLETS, each_tasklet, result->cycles_mt32, (double)result->cycles_mt32 / result->clocks_per_sec);
      printf("cpu,%u,cpu,%u,%lu,%.2e,sc32\n", NR_TASKLETS, each_tasklet, result->cycles_sc32, (double)result->cycles_sc32 / result->clocks_per_sec);
      printf("cpu,%u,cpu,%u,%lu,%.2e,lm32\n", NR_TASKLETS, each_tasklet, result->cycles_lm32, (double)result->cycles_lm32 / result->clocks_per_sec);
      printf("cpu,%u,cpu,%u,%lu,%.2e,pcg32\n", NR_TASKLETS, each_tasklet, result->cycles_pcg32, (double)result->cycles_pcg32 / result->clocks_per_sec);
      printf("cpu,%u,cpu,%u,%lu,%.2e,cha32\n", NR_TASKLETS, each_tasklet, result->cycles_cha32, (double)result->cycles_cha32 / result->clocks_per_sec);
    }

    return 0;
}

