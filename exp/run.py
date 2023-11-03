import os
import argparse

MAKE_CMD = "NR_DPUS=X NR_TASKLETS=Y make all"
RUN_CMD = "./bin/host >> X"

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("dpu_depth", type = int, default = 4)
    parser.add_argument("task_depth", type = int, default = 4)
    parser.add_argument('--dlinear', default=False, action=argparse.BooleanOptionalAction)
    parser.add_argument('--tlinear', default=False, action=argparse.BooleanOptionalAction)
    

    args = parser.parse_args()

    print("---=== DPU-Depth {} & TASK-Depth {} ===---\n".format(args.dpu_depth, args.task_depth))

    out = "data/out_dpus_{}_tasklets_{}.csv".format(args.dpu_depth, args.task_depth)

    os.system("echo 'nr_dpu,nr_tasklet,dpu_id,tasklet_id,cycles,time,algorithm' > X".replace("X", out));

    try: 
        os.mkdir("data")
    except OSError:
        pass

    dpus = [i for i in range(args.dpu_depth + 1)]
    if not args.dlinear:
        dpus = [1 << i for i in dpus]
    
    tasks = [i for i in range(args.task_depth + 1)]
    if not args.tlinear:
        tasks = [1 << i for i in tasks]

    for nr_dpus in dpus:
        for nr_task in tasks:
            if nr_dpus == 0 or nr_task == 0:
                continue
            
            make = MAKE_CMD.replace("X", str(nr_dpus))
            make = make.replace("Y", str(nr_task))

            run = RUN_CMD.replace("X", out)

            os.system("make clean")
            os.system(make)
            os.system(run)


if __name__ == "__main__":
    main()