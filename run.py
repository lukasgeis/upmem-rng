import os
import argparse

N = "1e9"

MAKE_CMD = "NR_DPUS=X NR_TASKLETS=Y N={} make dpu".format(N)
CPU_MAKE_CMD = "NR_TASKLETS=X N={} make cpu".format(N)
DPU_RUN_CMD = "./bin/host >> X"
CPU_RUN_CMD = "./bin/cpu >> X"

EXP_PARAMS = {
    "RNG": ("nr_dpu,nr_tasklet,dpu_id,tasklet_id,cycles,time,algorithm"),
    "SAMPLING": ("target,nr_tasklets,tasklet_id,time,algorithm")
}

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("experiment", type = str, default = "RNG")
    parser.add_argument("--dpus", type = int, default = 4)
    parser.add_argument("--tasks", type = int, default = 4)
    parser.add_argument('--dlinear', default=False, action=argparse.BooleanOptionalAction)
    parser.add_argument('--tlinear', default=False, action=argparse.BooleanOptionalAction)
    parser.add_argument('--cpu', default = False, action=argparse.BooleanOptionalAction)
    

    args = parser.parse_args()

    # Change Working Directory
    os.chdir(args.experiment)
    os.getcwd()

    print("---=== Experiment {} ===---\n".format(args.experiment))

    out = "../data/{}_dpus_{}_tasklets_{}.csv".format(args.experiment.lower(), args.dpus, args.tasks)

    os.system("echo {} > X".format(EXP_PARAMS[args.experiment]).replace("X", out));

    try: 
        os.mkdir("../data")
    except OSError:
        pass

    dpus = [i for i in range(args.dpus + 1)]
    if not args.dlinear:
        dpus = [1 << i for i in dpus]
    
    if args.experiment == "SAMPLING":
        dpus = [args.dpus]

    tasks = [i for i in range(args.tasks+ 1)]
    if not args.tlinear:
        tasks = [1 << i for i in tasks]

    for nr_dpus in dpus:
        for nr_task in tasks:
            if nr_dpus == 0 or nr_task == 0:
                continue
            
            make = MAKE_CMD.replace("X", str(nr_dpus))
            make = make.replace("Y", str(nr_task))

            run_dpu = DPU_RUN_CMD.replace("X", out)

            os.system("make clean")
            os.system(make)
            os.system(run_dpu)

            if args.cpu:
                cpu_nr_tasks = nr_dpus * nr_task
                if args.experiment == "SAMPLING":
                    cpu_nr_tasks = nr_task
                make = CPU_MAKE_CMD.replace("X", str(cpu_nr_tasks))

                os.system("make clean")
                os.system(make)
                run_cpu = CPU_RUN_CMD.replace("X", out)
                os.system(run_cpu)




if __name__ == "__main__":
    main()