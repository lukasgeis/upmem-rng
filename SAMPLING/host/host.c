#include <dpu.h>
#include <stdio.h>

#include "../../includes/results.h"

#define DPU_BINARY "./bin/dpu"

int main() {
  struct dpu_set_t set, dpu;

  DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &set));
  DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));

  // Transfer DPU-ID to DPU-BINARY
  uint32_t dpu_ids[NR_DPUS];
  for (unsigned i = 0; i < NR_DPUS; i++) {
      dpu_ids[i] = i;
  }
  uint32_t dpu_id;
  DPU_FOREACH(set, dpu, dpu_id) {
      DPU_ASSERT(dpu_prepare_xfer(dpu, &dpu_ids[dpu_id]));
  }
  DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "dpu_id", 0, sizeof(uint32_t), DPU_XFER_DEFAULT));

  // Launch DPU-BINARY
  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));

  sampling_dpu_result results[NR_DPUS];
  uint32_t each_dpu;
  DPU_FOREACH(set, dpu, each_dpu) {
      DPU_ASSERT(dpu_prepare_xfer(dpu, &results[each_dpu]));
  }
  DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_FROM_DPU, XSTR(DPU_RESULTS), 0, sizeof(sampling_dpu_result), DPU_XFER_DEFAULT));

  sampling_host_result host_results;

  for (unsigned int each_tasklet = 0; each_tasklet < NR_TASKLETS; each_tasklet++) {
    host_results.tasklet_results[each_tasklet].time_loop = 0;
    host_results.tasklet_results[each_tasklet].time_openbsd = 0;
    host_results.tasklet_results[each_tasklet].time_java = 0;
    host_results.tasklet_results[each_tasklet].time_lemire = 0;
    host_results.tasklet_results[each_tasklet].time_flips = 0;
    host_results.tasklet_results[each_tasklet].time_rr = 0;
    host_results.tasklet_results[each_tasklet].time_rrb = 0;
    host_results.tasklet_results[each_tasklet].time_rrf = 0;
  }

  DPU_FOREACH(set, dpu, each_dpu) {
    for (unsigned int each_tasklet = 0; each_tasklet < NR_TASKLETS; each_tasklet++) {
      sampling_tasklet_result *result = &results[each_dpu].tasklet_results[each_tasklet];
      sampling_host_tasklet_result *host_result = &host_results.tasklet_results[each_tasklet];

      host_result->time_loop += (double)result->cycles_loop / result->clocks_per_sec;
      host_result->time_openbsd += (double)result->cycles_openbsd / result->clocks_per_sec;
      host_result->time_java += (double)result->cycles_java / result->clocks_per_sec;
      host_result->time_lemire += (double)result->cycles_lemire / result->clocks_per_sec;
      host_result->time_flips += (double)result->cycles_flips / result->clocks_per_sec;
      host_result->time_rr += (double)result->cycles_rr / result->clocks_per_sec;
      host_result->time_rrb += (double)result->cycles_rrb / result->clocks_per_sec;
      host_result->time_rrf += (double)result->cycles_rrf / result->clocks_per_sec;
    }
  }

  for (unsigned int each_tasklet = 0; each_tasklet < NR_TASKLETS; each_tasklet++) {
    printf("dpu,%u,%u,%.2e,loop\n", NR_TASKLETS, each_tasklet, host_results.tasklet_results[each_tasklet].time_loop);
    printf("dpu,%u,%u,%.2e,openbsd\n", NR_TASKLETS, each_tasklet, host_results.tasklet_results[each_tasklet].time_openbsd);
    printf("dpu,%u,%u,%.2e,java\n", NR_TASKLETS, each_tasklet, host_results.tasklet_results[each_tasklet].time_java);
    printf("dpu,%u,%u,%.2e,lemire\n", NR_TASKLETS, each_tasklet, host_results.tasklet_results[each_tasklet].time_lemire);
    printf("dpu,%u,%u,%.2e,flips\n", NR_TASKLETS, each_tasklet, host_results.tasklet_results[each_tasklet].time_flips);
    printf("dpu,%u,%u,%.2e,rr\n", NR_TASKLETS, each_tasklet, host_results.tasklet_results[each_tasklet].time_rr);
    printf("dpu,%u,%u,%.2e,rrb\n", NR_TASKLETS, each_tasklet, host_results.tasklet_results[each_tasklet].time_rrb);
    printf("dpu,%u,%u,%.2e,rrf\n", NR_TASKLETS, each_tasklet, host_results.tasklet_results[each_tasklet].time_rrf);
  }


  DPU_ASSERT(dpu_free(set));

  return 0;
}
