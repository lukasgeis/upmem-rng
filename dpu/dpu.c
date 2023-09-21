#include <defs.h>
#include <perfcounter.h>
#include <stdio.h>

#include "../includes/rng.c"
#include "../includes/results.h"

#define N 1e4

__host dpu_results_t DPU_RESULTS;

int main() {
    uint32_t tasklet_id = me();
    uint32_t seed = tasklet_id;
    dpu_tasklet_result_t *result = &DPU_RESULTS.tasklet_results[tasklet_id];

    uint32_t num_cycles;

    if (tasklet_id == 0) {
        perfcounter_config(COUNT_CYCLES, true);
    }

    struct xs32 rng_xs32 = seed_xs32(seed);

    for (int i = 0; i < N; i++) {
        gen_xs32(&rng_xs32);    
    }

    num_cycles = perfcounter_get();
    result->cycles_xs32 = num_cycles;

    struct mt32 rng_mt32 = seed_mt32(seed);

    for (int i = 0; i < N; i++) {
        gen_mt32(&rng_mt32);    
    }

    num_cycles = perfcounter_get() - num_cycles;
    result->cycles_mt32 = num_cycles;

    struct sc32 rng_sc32 = seed_sc32(seed);

    for (int i = 0; i < N; i++) {
        gen_sc32(&rng_sc32);    
    }

    num_cycles = perfcounter_get() - num_cycles;
    result->cycles_sc32 = num_cycles;

    struct lm32 rng_lm32 = seed_lm32(seed);

    for (int i = 0; i < N; i++) {
        gen_lm32(&rng_lm32);    
    }

    num_cycles = perfcounter_get() - num_cycles;
    result->cycles_lm32 = num_cycles;

    result->clocks_per_sec = CLOCKS_PER_SEC;

    return 0;
}

