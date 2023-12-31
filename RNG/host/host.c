#include <dpu.h>
#include <stdio.h>

#include "../../includes/results.h"

#define DPU_BINARY "./bin/dpu"

int main() {
  struct dpu_set_t set, dpu;

  DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &set));
  DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));

  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));

  rng_dpu_result results[NR_DPUS];
  uint32_t each_dpu;
  DPU_FOREACH(set, dpu, each_dpu) {
      DPU_ASSERT(dpu_prepare_xfer(dpu, &results[each_dpu]));
  }
  DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_FROM_DPU, XSTR(DPU_RESULTS), 0, sizeof(rng_dpu_result), DPU_XFER_DEFAULT));

  DPU_FOREACH(set, dpu, each_dpu) {
    for (unsigned int each_tasklet = 0; each_tasklet < NR_TASKLETS; each_tasklet++) {
      rng_tasklet_result *result = &results[each_dpu].tasklet_results[each_tasklet];

      printf("%u,%u,%u,%u,%lu,%.2e,loop\n", NR_DPUS, NR_TASKLETS, each_dpu, each_tasklet, result->cycles_loop, (double)result->cycles_loop / result->clocks_per_sec);
      printf("%u,%u,%u,%u,%lu,%.2e,xs32\n", NR_DPUS, NR_TASKLETS, each_dpu, each_tasklet, result->cycles_xs32, (double)result->cycles_xs32 / result->clocks_per_sec);
      printf("%u,%u,%u,%u,%lu,%.2e,mt32\n", NR_DPUS, NR_TASKLETS, each_dpu, each_tasklet, result->cycles_mt32, (double)result->cycles_mt32 / result->clocks_per_sec);
      printf("%u,%u,%u,%u,%lu,%.2e,sc32\n", NR_DPUS, NR_TASKLETS, each_dpu, each_tasklet, result->cycles_sc32, (double)result->cycles_sc32 / result->clocks_per_sec);
      printf("%u,%u,%u,%u,%lu,%.2e,lm32\n", NR_DPUS, NR_TASKLETS, each_dpu, each_tasklet, result->cycles_lm32, (double)result->cycles_lm32 / result->clocks_per_sec);
      printf("%u,%u,%u,%u,%lu,%.2e,pcg32\n", NR_DPUS, NR_TASKLETS, each_dpu, each_tasklet, result->cycles_pcg32, (double)result->cycles_pcg32 / result->clocks_per_sec);
      printf("%u,%u,%u,%u,%lu,%.2e,cha32\n", NR_DPUS, NR_TASKLETS, each_dpu, each_tasklet, result->cycles_cha32, (double)result->cycles_cha32 / result->clocks_per_sec);
    }
  }

  DPU_ASSERT(dpu_free(set));

  return 0;
}
