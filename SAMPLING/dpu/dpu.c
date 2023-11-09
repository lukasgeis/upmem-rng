#include <barrier.h>
#include <defs.h>
#include <perfcounter.h>

#include "../../includes/sampling.c"
#include "../../includes/results.h"

#ifndef N
#define N 1e7
#endif

__host sampling_dpu_result DPU_RESULTS;

BARRIER_INIT(my_barrier, NR_TASKLETS);


static struct xs32 rngs[NR_TASKLETS];

uint32_t rng32(unsigned id) {
    return gen_xs32(&rngs[id]);
}


int main() {
    uint32_t tasklet_id = me();
    rngs[tasklet_id] = seed_xs32(tasklet_id + 1);    

    uint32_t dummy = 0;
    sampling_tasklet_result *result = &DPU_RESULTS.tasklet_results[tasklet_id];

    uint64_t num_cycles;

    if (tasklet_id == 0) {
        perfcounter_config(COUNT_CYCLES, true);
    }

    barrier_wait(&my_barrier);

    num_cycles = perfcounter_get();

    for (int i = 0; i < N; i++) {
        dummy ^= i;
    }

    result->cycles_loop = perfcounter_get() - num_cycles;
    num_cycles = perfcounter_get();

    for (int i = 2; i < N; i++) {
        dummy ^= openbsd(i, rng32, tasklet_id);  
    }

    result->cycles_openbsd = perfcounter_get() - num_cycles;
    num_cycles = perfcounter_get();

    for (int i = 2; i < N; i++) {
        dummy ^= java(i, rng32, tasklet_id);  
    }

    result->cycles_java = perfcounter_get() - num_cycles;
    num_cycles = perfcounter_get();

    for (int i = 2; i < N; i++) {
        dummy ^= lemire(i, rng32, tasklet_id);  
    }

    result->cycles_lemire = perfcounter_get() - num_cycles;
    num_cycles = perfcounter_get();

    for (int i = 2; i < N; i++) {
        dummy ^= flips(i, rng32, tasklet_id);  
    }

    result->cycles_flips = perfcounter_get() - num_cycles;
    num_cycles = perfcounter_get();

    for (int i = 2; i < N; i++) {
        dummy ^= rr(i, rng32, tasklet_id);  
    }

    result->cycles_rr = perfcounter_get() - num_cycles;

    result->clocks_per_sec = CLOCKS_PER_SEC;

    // Prevent Compiler-Optimization of previous Loops
    result->dummy = dummy;

    return 0;
}