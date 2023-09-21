import os
import argparse

MAKE_CMD = "NR_DPUS=X NR_TASKLETS=Y make all"
RUN_CMD = "./bin/host > X"

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("dpu_depth", type = int, default = 4)
    parser.add_argument("task_depth", type = int, default = 5)

    args = parser.parse_args()

    print("---=== DPU-Depth {} & TASK-Depth {} ===---\n".format(args.dpu_depth, args.task_depth))

    try: 
        os.mkdir("data")
    except OSError:
        pass

    for dpu_exp in range(args.dpu_depth + 1):
        for task_exp in range(args.task_depth + 1):
            nr_dpus = 1 << dpu_exp
            nr_task = 1 << task_exp

            make = MAKE_CMD.replace("X", str(nr_dpus))
            make = make.replace("Y", str(nr_task))

            out = "data/out_dpus_{}_tasklets_{}.csv".format(nr_dpus, nr_task)
            run = RUN_CMD.replace("X", out)

            os.system("make clean")
            os.system(make)
            os.system(run)


if __name__ == "__main__":
    main()