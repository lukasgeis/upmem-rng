#include <barrier.h>
#include <defs.h>
#include <perfcounter.h>
#include <stdio.h>

#include "../includes/rng.c"
#include "../includes/results.h"

__host dpu_results_t DPU_RESULTS;

BARRIER_INIT(my_barrier, NR_TASKLETS);

int main() {
    uint32_t tasklet_id = me();
    uint32_t seed = tasklet_id;
    uint32_t dummy = 0;
    dpu_tasklet_result_t *result = &DPU_RESULTS.tasklet_results[tasklet_id];

    uint64_t num_cycles;

    if (tasklet_id == 0) {
        perfcounter_config(COUNT_CYCLES, true);
    }

    barrier_wait(&my_barrier);

    num_cycles = perfcounter_get();

    for (int i = 0; i < N; i++) {
        dummy ^= i;
    }

    result->cycles_null = perfcounter_get() - num_cycles;
    num_cycles = perfcounter_get();


    struct xs32 rng_xs32 = seed_xs32(seed);

    for (int i = 0; i < N; i++) {
        dummy ^= gen_xs32(&rng_xs32);    
    }

    result->cycles_xs32 = perfcounter_get() - num_cycles;
    num_cycles = perfcounter_get();

    struct mt32 rng_mt32 = seed_mt32(seed);

    for (int i = 0; i < N; i++) {
        dummy ^= gen_mt32(&rng_mt32);    
    }

    result->cycles_mt32 = perfcounter_get() - num_cycles;
    num_cycles = perfcounter_get();

    struct sc32 rng_sc32 = seed_sc32(seed);

    for (int i = 0; i < N; i++) {
        dummy ^= gen_sc32(&rng_sc32);    
    }

    result->cycles_sc32 = perfcounter_get() - num_cycles;
    num_cycles = perfcounter_get();

    struct lm32 rng_lm32 = seed_lm32(seed);

    for (int i = 0; i < N; i++) {
        dummy ^= gen_lm32(&rng_lm32);    
    }

    result->cycles_lm32 = perfcounter_get() - num_cycles;
    num_cycles = perfcounter_get();

    struct pcg32 rng_pcg32 = seed_pcg32(seed);

    for (int i = 0; i < N; i++) {
        dummy ^= gen_pcg32(&rng_pcg32);    
    }

    result->cycles_pcg32 = perfcounter_get() - num_cycles;
    num_cycles = perfcounter_get();

    struct cha32 rng_cha32 = seed_cha32(seed);

    for (int i = 0; i < N; i++) {
        dummy ^= gen_cha32(&rng_cha32);    
    }

    result->cycles_cha32 = perfcounter_get() - num_cycles;


    result->clocks_per_sec = CLOCKS_PER_SEC;

    // Prevent Compiler-Optimization of previous Loops
    result->dummy = dummy;

    return 0;
}

