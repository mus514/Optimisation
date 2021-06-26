import json

# Using readlines()
file1 = open('NameInput.txt', 'r')
Lines = file1.read().splitlines()

for i in range(0,16):

    # List of files in complete directory
    file_list = []

    if i == 0 :
        for l in Lines:
            new = l + '.json'
            file_list.append(new)
    else :
        for l in Lines:
            new = l + "_" + str(i) + '.json'
            file_list.append(new)

    # Loop to print each filename separately
    for file in file_list:
        filepath = './in/' + str(i) + '/' + file
        try:
            f = open(filepath)

            with open(filepath, "r") as read_file:
                json_object = json.load(read_file)

                data = json_object['SolutionInfo']

                f = open("Output.txt", "a")

                if data.get('IntVio', -1) != -1:
                    f.write(filepath + ' ')
                    f.write(data['ObjVal'] + ' ')
                    f.write(data['ObjBound'] + ' ')
                    f.write(data['MIPGap'] + ' ')
                    f.write(data['Runtime'] + ' ')
                    f.write(data['NodeCount'] + ' ')
                    f.write(data['IterCount'] + ' ')
                    f.write(str(data['SolCount']) + ' ')
                    f.write(data['IntVio'] + ' ')
                    f.write(data['BoundVio'] + ' ')
                    f.write(data['ConstrVio'] + ' ' + '\n')
                else:
                    f.write(filepath + ' ')
                    f.write(data['ObjVal'] + ' ')
                    f.write(data['ObjBound'] + ' ')
                    f.write(data['MIPGap'] + ' ')
                    f.write(data['Runtime'] + ' ')
                    f.write(data['NodeCount'] + ' ')
                    f.write(data['IterCount'] + ' ')
                    f.write(str(data['SolCount']) + ' ' + '\n')

                f.close()
                read_file.close()

        except IOError:

            print(file)

            f = open("Output.txt", "a")
            f.write(filepath + '\n')
            f.close()

