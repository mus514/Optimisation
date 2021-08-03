import pandas as pd
import os.path
import shutil, os


def make_cdf(sheet, folder, vehicles, ordering):
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

##
## Creat different folders and divied them in many part depending on number of missing instances
# in each part we copy maximun 8 instances,
##

def copy_missing(file, sheet):

    data = pd.read_excel(file+".xlsx", sheet_name= sheet)
    ordering = data["ordering"].unique()

    # Creating the folders
    try:
        os.mkdir(f"irp_lp_solver-master/input/missing_{sheet}")
        os.mkdir(f"irp_lp_solver-master/output/missing_{sheet}")
        for i in ordering:
            if(i == 0):
                os.mkdir(f"irp_lp_solver-master/input/missing_{sheet}/Original")
                os.mkdir(f"irp_lp_solver-master/output/missing_{sheet}/Original")
                for j in data[data["ordering"] == 0]["vehicles"].unique():
                    os.mkdir(f"irp_lp_solver-master/input/missing_{sheet}/Original/V{j}")
                    os.mkdir(f"irp_lp_solver-master/output/missing_{sheet}/Original/V{j}")

                    temp = data[data["ordering"] == 0]
                    temp = temp[temp["vehicles"] == j]

                    if len(temp) <= 8 :
                        os.mkdir(f"irp_lp_solver-master/input/missing_{sheet}/Original/V{j}/{0}")

                    if len(temp) > 8:

                        if(len(temp) % 8 == 0):
                            l = int((len(temp) - len(temp) % 8) / 8)
                            for k in range(l):
                                os.mkdir(f"irp_lp_solver-master/input/missing_{sheet}/Original/V{j}/{k}")


                        else :
                            l = int((len(temp) - len(temp) % 8) / 8)
                            for k in range(l + 1):
                                os.mkdir(f"irp_lp_solver-master/input/missing_{sheet}/Original/V{j}/{k}")


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

                    if len(temp) > 8:

                        if (len(temp) % 8 == 0):
                            l = int((len(temp) - len(temp) % 8) / 8)
                            for k in range(l):
                                os.mkdir(f"irp_lp_solver-master/input/missing_{sheet}/{i}/V{j}/{k}")


                        else:
                            l = int((len(temp) - len(temp) % 8) / 8)
                            for k in range(l + 1):
                                os.mkdir(f"irp_lp_solver-master/input/missing_{sheet}/{i}/V{j}/{k}")

    except:
        print("Folder Already exist, delete all the folders and restart !!!")

    # Coping and dividing the instances
    for i in ordering:
        if(i == 0):
            for j in data[data["ordering"] == 0]["vehicles"].unique():
                temp = data[data["ordering"] == 0]
                temp = temp[temp["vehicles"] == j]
                temp = list(temp["name"])

                if len(temp) <= 8:
                    for k in temp:
                        shutil.copy(f"irp_lp_solver-master/input/Instances/Original/{k}",
                                    f"irp_lp_solver-master/input/missing_{sheet}/Original/V{j}/0")

                temp_1 = []

                if len(temp) > 8:
                    l = int((len(temp) - len(temp) % 8) / 8)

                    if (len(temp) % 8 == 0):
                        for k in range(0, l*8, 8):
                            temp_1.append(temp[k:k+8])

                    else:
                        for k in range(0, l*8, 8):
                            temp_1.append(temp[k:k+8])
                        temp_1.append(temp[l*8:len(temp)])

                    for k in range(len(temp_1)):
                        for n in temp_1[k]:
                            shutil.copy(f"irp_lp_solver-master/input/Instances/Original/{n}",
                                        f"irp_lp_solver-master/input/missing_{sheet}/Original/V{j}/{k}")

        else:
            for j in data[data["ordering"] == i]["vehicles"].unique():
                temp = data[data["ordering"] == i]
                temp = temp[temp["vehicles"] == j]
                temp = list(temp["name"])

                if len(temp) <= 8:
                    for k in temp:
                        shutil.copy(f"irp_lp_solver-master/input/Instances/{i}/{k}",
                                    f"irp_lp_solver-master/input/missing_{sheet}/{i}/V{j}/0")

                temp_1 = []

                if len(temp) > 8:
                    l = int((len(temp) - len(temp) % 8) / 8)

                    if (len(temp) % 8 == 0):
                        for k in range(0, l*8, 8):
                            temp_1.append(temp[k:k+8])

                    else:
                        for k in range(0, l*8, 8):
                            temp_1.append(temp[k:k+8])
                        temp_1.append(temp[l*8:len(temp)])

                    for k in range(len(temp_1)):
                        for n in temp_1[k]:
                            shutil.copy(f"irp_lp_solver-master/input/Instances/{i}/{n}",
                                        f"irp_lp_solver-master/input/missing_{sheet}/{i}/V{j}/{k}")

    return (data)


print(make_cdf("SF", 2, 5, "Original"))

#if __name__ == "__main__":

   #copy_missing("ARF_SF-missing", "SF")
   #copy_missing("SBC-missing", "cos")