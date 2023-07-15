import numpy as np
import os
import matplotlib.pyplot as plt
from cmath import log, log10

os.system("chmod +x get_hosts.sh")
os.system("./get_hosts.sh")

hosts=[]
with open('hosts.txt') as my_file:
    hosts = my_file.readlines()

for i in range(len(hosts)):
    hosts[i] = hosts[i][:len(hosts[i] )-1]

#data_size = np.array([1024,102400,1048576,8388608,33554432,67108864,134217728])
data_size= np.array([100,400,1600,6400,25600,102400,409600])
os.system("mpicc -o hello code.c")

os.system("rm output.txt")
os.system("touch output.txt")
for i in range(len(data_size)):
   for j in range(0,len(hosts),2):
        for k in range(4):
           os.system("mpirun -np 2 -hosts {0},{1} ./hello {2} 2> error.txt >> output.txt".format(hosts[j],hosts[j+1],data_size[i]))

os.system("python3 analysis.py")