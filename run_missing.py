# Mustapha Bouhsen

import pandas as pd
import os.path
import shutil, os


def make_cfg(sheet, folder, vehicles, ordering):
    """

    :return: cfg scripte
    """

    scripte = f"""################################################################################
#
# Classic IRP solver: example of the input configuration file.
#
################################################################################
#
# --- Execution configuration options ---
#
# ============================= General parameters =============================
# (std::string): single instance path or instances directory (all instances from
# this directory are executed in batch).
# (single instance execution)
# instance_path = ./input/missing_{sheet}/{ordering}/V{vehicles}/{folder}
# (batch execution)
#instance_path = ./Instances/Original/
instance_path =  ./input/missing_{sheet}/{ordering}/V{vehicles}/{folder}
#
# (std::string): directory path where all methods output (results, statistics,
# etc) will be stored. A folder is created if it does not exist.
output_dir = ./output/missing_{sheet}/{ordering}/V{vehicles}
# ============================= Solver parameters ==============================
#
# (bool): silences (or not) the solver output.
solver_show_log = false
#
# (unsigned int): solver maximum execution time (in seconds) in every solver
# execution. Set 'unlimited' to don't limit it.
solver_time_limit = 3600
#
# (unsigned int): number of threads used by the solver. Se 'max' to use all the
# machine threads.
solver_nb_threads = 2
#
# ============================== Model parameters ==============================
#
# (unsigned int): number of vehicles (K).
nb_vehicles = {vehicles}
#
# (unsigned int): execution model invetory policy:
#   0: maximum level inventory policy (ML).
#   1: order-up to level inventory policy (OU).
model_policy = 1
# (unsigned int): subtour elimination strategy :
#   0: adds the standard subtour elimination constraints to the model.
#   1: adds lazy and cut constraints from CVRPSEP package.
sec_strategy = 1
"""
    return scripte


def make_sh(sheet, ordering, time, end, default="SLURM_ARRAY_TASK_ID"):
    """

    :return: Sh scripte
    """
    scripte = f"""
#!/bin/bash
#SBATCH --account=def-mardar
#SBATCH --mail-user=bouhsen.m@gmail.com
#SBATCH --mail-type=FAIL
#SBATCH --job-name=irp-solver
#SBATCH --time={time}:00:00
#SBATCH --cpus-per-task=2
#SBATCH --mem-per-cpu=16G
#SBATCH --output=log/%x-%j.out
#SBATCH --array=1-{end}
module load StdEnv/2020
module loadg gcc/9.3.0
module load gurobi
echo "Starting task $SLURM_ARRAY_TASK_ID"
./build/irp_solver -f ./cdf_{sheet}/{sheet}-{ordering}-$"'{default}'".cfg
    """
    return scripte


def run_scripte(vector, sheet):
    """

    :return: run scripte
    """
    file = open("irp_lp_solver-master/run.sh", 'w')
    file.write("#!/bin/bash \n \n \n")
    for i in vector:
        file.write(f"sbatch sh_{sheet}/{i} \n")
    file.close()


##
## Creat different folders and divied them in many part depending on number of missing instances
# in each part we copy maximun 8 instances,
##

def make_missing(file, sheet):
    # Import data
    data = pd.read_excel(file + ".xlsx", sheet_name=sheet)

    # Sort by all the orderings
    ordering = data["ordering"].unique()

    try:
        # Creating the folders for the input, output, cfg and sh
        os.mkdir(f"irp_lp_solver-master/input/missing_{sheet}")
        os.mkdir(f"irp_lp_solver-master/output/missing_{sheet}")
        os.mkdir(f"irp_lp_solver-master/cfg_{sheet}")
        os.mkdir(f"irp_lp_solver-master/sh_{sheet}")
        counter = 1
        end = []

        for i in ordering:

            # case if the ordering is "Original"
            if i == 0:
                # create empty folder for input and output
                os.mkdir(f"irp_lp_solver-master/input/missing_{sheet}/Original")
                os.mkdir(f"irp_lp_solver-master/output/missing_{sheet}/Original")

                for j in data[data["ordering"] == 0]["vehicles"].unique():
                    # field the empty Original folder by the missing vehicles
                    os.mkdir(f"irp_lp_solver-master/input/missing_{sheet}/Original/V{j}")
                    os.mkdir(f"irp_lp_solver-master/output/missing_{sheet}/Original/V{j}")

                    temp = data[data["ordering"] == 0]
                    temp = temp[temp["vehicles"] == j]

                    # we dived the missing instances for each vehicles by group of 8. If we have 8 or less we create one
                    # folder. Else, we calculate (number of missing instances modulo 8), if the rest is not 0 we add
                    # another folder to put the rest. In the same time we create the cfg files and we put them in the
                    # folders

                    if len(temp) <= 8:
                        os.mkdir(f"irp_lp_solver-master/input/missing_{sheet}/Original/V{j}/{0}")
                        file_name = f"irp_lp_solver-master/cfg_{sheet}/{sheet}-Original-{counter}.cfg"
                        file_name = open(file_name, 'a')
                        file_name.write(make_cfg(sheet, 0, j, "Original"))
                        file_name.close()
                        counter += 1

                    if len(temp) > 8:

                        if len(temp) % 8 == 0:
                            l = int((len(temp) - len(temp) % 8) / 8)
                            for k in range(l):
                                os.mkdir(f"irp_lp_solver-master/input/missing_{sheet}/Original/V{j}/{k}")
                                file_name = f"irp_lp_solver-master/cfg_{sheet}/{sheet}-Original-{counter}.cfg"
                                file_name = open(file_name, 'a')
                                file_name.write(make_cfg(sheet, k, j, "Original"))
                                file_name.close()
                                counter += 1

                        else:
                            l = int((len(temp) - len(temp) % 8) / 8)
                            for k in range(l + 1):
                                os.mkdir(f"irp_lp_solver-master/input/missing_{sheet}/Original/V{j}/{k}")
                                file_name = f"irp_lp_solver-master/cfg_{sheet}/{sheet}-Original-{counter}.cfg"
                                file_name = open(file_name, 'a')
                                file_name.write(make_cfg(sheet, k, j, "Original"))
                                file_name.close()
                                counter += 1

            # Other ordering from 1 to 15. Same logic as Original case
            else:
                os.mkdir(f"irp_lp_solver-master/input/missing_{sheet}/{i}")
                os.mkdir(f"irp_lp_solver-master/output/missing_{sheet}/{i}")

                for j in data[data["ordering"] == i]["vehicles"].unique():
                    os.mkdir(f"irp_lp_solver-master/input/missing_{sheet}/{i}/V{j}")
                    os.mkdir(f"irp_lp_solver-master/output/missing_{sheet}/{i}/V{j}")

                    temp = data[data["ordering"] == i]
                    temp = temp[temp["vehicles"] == j]

                    if len(temp) <= 8:
                        os.mkdir(f"irp_lp_solver-master/input/missing_{sheet}/{i}/V{j}/{0}")
                        file_name = f"irp_lp_solver-master/cfg_{sheet}/{sheet}-{i}-{counter}.cfg"
                        file_name = open(file_name, 'a')
                        file_name.write(make_cfg(sheet, 0, j, i))
                        file_name.close()
                        counter += 1

                    if len(temp) > 8:

                        if (len(temp) % 8 == 0):
                            l = int((len(temp) - len(temp) % 8) / 8)
                            for k in range(l):
                                os.mkdir(f"irp_lp_solver-master/input/missing_{sheet}/{i}/V{j}/{k}")
                                file_name = f"irp_lp_solver-master/cfg_{sheet}/{sheet}-{i}-{counter}.cfg"
                                file_name = open(file_name, 'a')
                                file_name.write(make_cfg(sheet, k, j, i))
                                file_name.close()
                                counter += 1


                        else:
                            l = int((len(temp) - len(temp) % 8) / 8)
                            for k in range(l + 1):
                                os.mkdir(f"irp_lp_solver-master/input/missing_{sheet}/{i}/V{j}/{k}")
                                file_name = f"irp_lp_solver-master/cfg_{sheet}/{sheet}-{i}-{counter}.cfg"
                                file_name = open(file_name, 'a')
                                file_name.write(make_cfg(sheet, k, j, i))
                                file_name.close()
                                counter += 1

            end.append(counter - 1)
            counter = 1

            # Coping and dividing the instances. We took the instance from the Instance in input and we copy them to the
            # specific folder we already create in the previews code

            time = []  # List of the times we need to run in CalculCanada
            counter = 0;

            for i in ordering:
                # case if the ordering is "Original"
                if (i == 0):

                    for j in data[data["ordering"] == 0]["vehicles"].unique():
                        temp = data[data["ordering"] == 0]
                        temp = temp[temp["vehicles"] == j]
                        temp = list(temp["name"])

                        # Same logic as the previews code, We copy the instances by 8 or less
                        if len(temp) <= 8:
                            for k in temp:
                                shutil.copy(f"irp_lp_solver-master/input/Instances/Original/{k}",
                                            f"irp_lp_solver-master/input/missing_{sheet}/Original/V{j}/0")
                                counter += 1

                        temp_1 = []  # List of the names of the missing instances

                        if len(temp) > 8:
                            l = int((len(temp) - len(temp) % 8) / 8)

                            if len(temp) % 8 == 0:
                                for k in range(0, l * 8, 8):
                                    temp_1.append(temp[k:k + 8])

                            else:
                                for k in range(0, l * 8, 8):
                                    temp_1.append(temp[k:k + 8])
                                temp_1.append(temp[l * 8:len(temp)])

                            for k in range(len(temp_1)):
                                for n in temp_1[k]:
                                    shutil.copy(f"irp_lp_solver-master/input/Instances/Original/{n}",
                                                f"irp_lp_solver-master/input/missing_{sheet}/Original/V{j}/{k}")
                                    counter += 1

                # Other ordering from 1 to 15. Same logic as Original case
                else:
                    for j in data[data["ordering"] == i]["vehicles"].unique():
                        temp = data[data["ordering"] == i]
                        temp = temp[temp["vehicles"] == j]
                        temp = list(temp["name"])

                        if len(temp) <= 8:
                            for k in temp:
                                shutil.copy(f"irp_lp_solver-master/input/Instances/{i}/{k}",
                                            f"irp_lp_solver-master/input/missing_{sheet}/{i}/V{j}/0")
                                counter += 1

                        temp_1 = []

                        if len(temp) > 8:
                            l = int((len(temp) - len(temp) % 8) / 8)

                            if len(temp) % 8 == 0:
                                for k in range(0, l * 8, 8):
                                    temp_1.append(temp[k:k + 8])

                            else:
                                for k in range(0, l * 8, 8):
                                    temp_1.append(temp[k:k + 8])
                                temp_1.append(temp[l * 8:len(temp)])

                            for k in range(len(temp_1)):
                                for n in temp_1[k]:
                                    shutil.copy(f"irp_lp_solver-master/input/Instances/{i}/{n}",
                                                f"irp_lp_solver-master/input/missing_{sheet}/{i}/V{j}/{k}")
                                    counter += 1
                time.append(counter)
                counter = 0

            # create .sh file
            counter = 0
            file = []
            for i in ordering:
                if (i == 0):
                    file_name = f"irp_lp_solver-master/sh_{sheet}/{sheet}-Original-1-{end[counter]}.sh"
                    file.append(f"{sheet}-Original-1-{end[counter]}.sh")
                    file_name = open(file_name, 'a')
                    file_name.write(make_sh(sheet, "Original", time[counter], end[counter]))
                    file_name.close()

                else:
                    file_name = f"irp_lp_solver-master/sh_{sheet}/{sheet}-{i}-1-{end[counter]}.sh"
                    file.append(f"{sheet}-{i}-1-{end[counter]}.sh")
                    file_name = open(file_name, 'a')
                    file_name.write(make_sh(sheet, i, time[counter], end[counter]))
                    file_name.close()

                counter += 1

            # Create the run scipte
            run_scripte(file, sheet)


    except:
        print("Folder Already exist, delete all the folders and restart !!!")

    print("End")

if __name__ == "__main__":
    copy_missing("ARF_SF-missing", "SF")
#    #copy_missing("SBC-missing", "cos")