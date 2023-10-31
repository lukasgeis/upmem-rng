import argparse

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("dpu_depth", type = int, default = 4)
    parser.add_argument("task_depth", type = int, default = 4)
    parser.add_argument('--dlinear', default=False, action=argparse.BooleanOptionalAction)
    parser.add_argument('--tlinear', default=False, action=argparse.BooleanOptionalAction)
    parser.add_argument('--full', default=False, action=argparse.BooleanOptionalAction)

    args = parser.parse_args()

    print("DPUS,TASKS,CYCLES,TIME,ALGORITHM")

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

            file = "data/out_dpus_{}_tasklets_{}.csv".format(nr_dpus, nr_task)
            with open(file, "r") as data:
                lines = [line.rstrip("\n") for line in data.readlines()[1:]]
                if args.full:
                    for line in lines:
                        print("{},{},{}".format(nr_dpus, nr_task, ",".join(line.split(",")[2:])))
                else:
                    cycles = {}
                    times = {}
                    counter = 0
                    for line in lines:
                        cnt = line.split(",")[2:]
                        if cnt[2] not in cycles:
                            cycles[cnt[2]] = 0
                        if cnt[2] not in times:
                            times[cnt[2]] = 0.0
                        cycles[cnt[2]] += int(cnt[0])
                        times[cnt[2]] += float(cnt[1])
                        counter += 1
                    
                    for algo in cycles:
                        print("{},{},{},{},{}".format(nr_dpus, nr_task, cycles[algo] / counter, times[algo] / counter, algo))
 

if __name__ == "__main__":
    main()