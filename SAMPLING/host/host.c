#include <dpu.h>
#include <stdio.h>

#include "../../includes/results.h"

#define DPU_BINARY "./bin/dpu"

int main() {
  struct dpu_set_t set, dpu;

  DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &set));
  DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));

  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));

  sampling_dpu_result results[NR_DPUS];
  uint32_t each_dpu;
  DPU_FOREACH(set, dpu, each_dpu) {
      DPU_ASSERT(dpu_prepare_xfer(dpu, &results[each_dpu]));
  }
  DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_FROM_DPU, XSTR(DPU_RESULTS), 0, sizeof(sampling_dpu_result), DPU_XFER_DEFAULT));

  DPU_FOREACH(set, dpu, each_dpu) {
    for (unsigned int each_tasklet = 0; each_tasklet < NR_TASKLETS; each_tasklet++) {
      sampling_tasklet_result *result = &results[each_dpu].tasklet_results[each_tasklet];

      printf("%u,%u,%u,%u,%lu,%.2e,loop\n", NR_DPUS, NR_TASKLETS, each_dpu, each_tasklet, result->cycles_loop, (double)result->cycles_loop / result->clocks_per_sec);
      printf("%u,%u,%u,%u,%lu,%.2e,openbsd\n", NR_DPUS, NR_TASKLETS, each_dpu, each_tasklet, result->cycles_openbsd, (double)result->cycles_openbsd / result->clocks_per_sec);
      printf("%u,%u,%u,%u,%lu,%.2e,java\n", NR_DPUS, NR_TASKLETS, each_dpu, each_tasklet, result->cycles_java, (double)result->cycles_java / result->clocks_per_sec);
      printf("%u,%u,%u,%u,%lu,%.2e,lemire\n", NR_DPUS, NR_TASKLETS, each_dpu, each_tasklet, result->cycles_lemire, (double)result->cycles_lemire / result->clocks_per_sec);
      printf("%u,%u,%u,%u,%lu,%.2e,flips\n", NR_DPUS, NR_TASKLETS, each_dpu, each_tasklet, result->cycles_flips, (double)result->cycles_flips / result->clocks_per_sec);
      printf("%u,%u,%u,%u,%lu,%.2e,rr\n", NR_DPUS, NR_TASKLETS, each_dpu, each_tasklet, result->cycles_rr, (double)result->cycles_rr / result->clocks_per_sec);
    }
  }

  DPU_ASSERT(dpu_free(set));

  return 0;
}
