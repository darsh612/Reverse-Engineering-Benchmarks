import os
import subprocess

# define the range of branches
branch_min = 256
branch_max = 8192

# set other parameters
iterations = '500' # number of iterations
distance = '64' # max distance in bytes between two subsequent branch instruction

branches = branch_min
while branches <= branch_max:
    branch = str(branches)

    # generate C code
    subprocess.run(['btb_char', iterations, distance, branch])

    # file name of the generated C program
    file_name = f"T1I5H64B{branch}.cpp"
    # name of the executable file
    ex_name = f"T1I5H64B{branches}"

    # compile C++ code
    subprocess.run(['g++', '-O0', file_name, '-o', ex_name])

    # run the executable file
    subprocess.run([ex_name])

    branches += 256  # double the value on each iteration