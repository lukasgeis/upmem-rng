// Heavily based on https://github.com/upmem/dpu_demo/blob/sdk-2023.2/checksum/common/common.h

#define XSTR(x) STR(x)
#define STR(x) #x

#define DPU_RESULTS dpu_results

#include <stdint.h>

typedef struct {
    uint64_t cycles_loop;
    uint64_t cycles_xs32;
    uint64_t cycles_mt32;
    uint64_t cycles_sc32;
    uint64_t cycles_lm32;
    uint64_t cycles_pcg32;
    uint64_t cycles_cha32;
    uint64_t clocks_per_sec;

    // Prevent Compiler-Optimization
    uint32_t dummy;
} rng_tasklet_result;

typedef struct {
    rng_tasklet_result tasklet_results[NR_TASKLETS];
} rng_dpu_result;

typedef struct {
    uint64_t cycles_loop;
    uint64_t cycles_openbsd;
    uint64_t cycles_java;
    uint64_t cycles_lemire;
    uint64_t cycles_flips;
    uint64_t cycles_rr;
    uint64_t cycles_rrb;
    uint64_t cycles_rrf;
    uint64_t clocks_per_sec;

    // Prevent Compiler-Optimization
    uint32_t dummy;
} sampling_tasklet_result;

typedef struct {
    sampling_tasklet_result tasklet_results[NR_TASKLETS];
} sampling_dpu_result;

typedef struct {
    double time_loop;
    double time_openbsd;
    double time_java;
    double time_lemire;
    double time_flips;
    double time_rr;
    double time_rrb;
    double time_rrf;
} sampling_host_tasklet_result;

typedef struct {
    sampling_host_tasklet_result tasklet_results[NR_TASKLETS];
} sampling_host_result;