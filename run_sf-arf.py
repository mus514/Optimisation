import pandas as pd
import os.path
import shutil, os


def copy_missing(file, sheet):

    data = pd.read_excel(file+".xlsx", sheet_name= sheet)
    ordering = data["ordering"].unique()

    try:
        for i in ordering:
            if(i == 0):
                os.mkdir(f"irp_lp_solver-master/input/missing/Original")
                for j in data[data["ordering"] == 0]["vehicles"].unique():
                    os.mkdir(f"irp_lp_solver-master/input/missing/Original/V{j}")

                    temp = data[data["ordering"] == 0]
                    temp = temp[temp["vehicles"] == j]

                    if len(temp) <= 8 :
                        os.mkdir(f"irp_lp_solver-master/input/missing/Original/V{j}/{0}")

                    if len(temp) > 8:

                        if(len(temp) % 8 == 0):
                            l = int((len(temp) - len(temp) % 8) / 8)
                            for k in range(l):
                                os.mkdir(f"irp_lp_solver-master/input/missing/Original/V{j}/{k}")


                        else :
                            l = int((len(temp) - len(temp) % 8) / 8)
                            for k in range(l + 1):
                                os.mkdir(f"irp_lp_solver-master/input/missing/Original/V{j}/{k}")


            else:
                os.mkdir(f"irp_lp_solver-master/input/missing/{i}")
                for j in data[data["ordering"] == i]["vehicles"].unique():
                    os.mkdir(f"irp_lp_solver-master/input/missing/{i}/V{j}")

                    temp = data[data["ordering"] == i]
                    temp = temp[temp["vehicles"] == j]

                    if len(temp) <= 8:
                        os.mkdir(f"irp_lp_solver-master/input/missing/{i}/V{j}/{0}")

                    if len(temp) > 8:

                        if (len(temp) % 8 == 0):
                            l = int((len(temp) - len(temp) % 8) / 8)
                            for k in range(l):
                                os.mkdir(f"irp_lp_solver-master/input/missing/{i}/V{j}/{k}")


                        else:
                            l = int((len(temp) - len(temp) % 8) / 8)
                            for k in range(l + 1):
                                os.mkdir(f"irp_lp_solver-master/input/missing/{i}/V{j}/{k}")

    except:
        print("Folder Already exist, delete all the folders and restart !!!")

    for i in ordering:
        if(i == 0):
            for j in data[data["ordering"] == 0]["vehicles"].unique():
                temp = data[data["ordering"] == 0]
                temp = temp[temp["vehicles"] == j]
                temp = list(temp["name"])

                if len(temp) <= 8:
                    for k in temp:
                        shutil.copy(f"irp_lp_solver-master/input/Instances/Original/{k}",
                                    f"irp_lp_solver-master/input/missing/Original/V{j}/0")

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
                                        f"irp_lp_solver-master/input/missing/Original/V{j}/{k}")

        else:
            for j in data[data["ordering"] == i]["vehicles"].unique():
                temp = data[data["ordering"] == i]
                temp = temp[temp["vehicles"] == j]
                temp = list(temp["name"])

                if len(temp) <= 8:
                    for k in temp:
                        shutil.copy(f"irp_lp_solver-master/input/Instances/{i}/{k}",
                                    f"irp_lp_solver-master/input/missing/{i}/V{j}/0")

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
                                        f"irp_lp_solver-master/input/missing/{i}/V{j}/{k}")




    return (data)

