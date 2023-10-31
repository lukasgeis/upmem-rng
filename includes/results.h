// Heavily based on https://github.com/upmem/dpu_demo/blob/sdk-2023.2/checksum/common/common.h

#define XSTR(x) STR(x)
#define STR(x) #x

#define DPU_RESULTS dpu_results

#include <stdint.h>

typedef struct {
    uint64_t cycles_xs32;
    uint64_t cycles_mt32;
    uint64_t cycles_sc32;
    uint64_t cycles_lm32;
    uint64_t clocks_per_sec;

    // Prevent Compiler-Optimization in `../dpu/dpu.c`
    uint32_t fval_xs32;
    uint32_t fval_mt32;
    uint32_t fval_sc32;
    uint32_t fval_lm32;
} dpu_tasklet_result_t;

typedef struct {
    dpu_tasklet_result_t tasklet_results[NR_TASKLETS];
} dpu_results_t;

