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
        print("Folder Already exist")

    return (data)











data = copy_missing("ARF_SF-missing" ,"SF")

data = data[data["ordering"] == 1]
data = data[data["vehicles"] == 5]

print(len(data))

# x = [[[i for i in range(2)] for j in range(5)] for k in range(15)]



#print(y)
# os.mkdir("irp_lp_solver-master/input/Instances/Original/allo")
#shutil.copy("test.txt", "allo")