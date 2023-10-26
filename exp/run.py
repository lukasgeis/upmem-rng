import os
import argparse

MAKE_CMD = "NR_DPUS=X NR_TASKLETS=Y make all"
RUN_CMD = "./bin/host > X"
COMPRESS_CMD = "python3 exp/compress.py X Y > Z"
COMPRESS_FULL_CMD = "python3 exp/compress.py X Y --full > Z"

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("dpu_depth", type = int, default = 4)
    parser.add_argument("task_depth", type = int, default = 4)
    parser.add_argument('--compress', default=False, action=argparse.BooleanOptionalAction)
    parser.add_argument('--cleanup', default=False, action=argparse.BooleanOptionalAction)
    

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

    if args.compress:
        compress = COMPRESS_CMD.replace(
            "X", str(args.dpu_depth)
        ).replace(
            "Y", str(args.task_depth)
        ).replace(
            "Z", "data/exp_{}_{}.csv".format(args.dpu_depth, args.task_depth)
        )

        compress_full = COMPRESS_FULL_CMD.replace(
            "X", str(args.dpu_depth)
        ).replace(
            "Y", str(args.task_depth)
        ).replace(
            "Z", "data/exp_{}_{}_full.csv".format(args.dpu_depth, args.task_depth)
        )

        os.system(compress)
        os.system(compress_full)

    if args.cleanup:
        cleanup = "rm -r data/out*"
        os.system(cleanup)



if __name__ == "__main__":
    main()