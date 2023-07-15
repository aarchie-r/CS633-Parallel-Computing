import matplotlib.pyplot as plt
import numpy as np
Y = []
with open('output.txt') as my_file:
    Y = my_file.readlines()

for i in range(len(Y)):
    Y[i] = float(Y[i][:len(Y[i] )-1])

a_split=np.array_split(Y, 7)
errors= [np.max(arr)-np.min(arr) for arr in a_split]
a_split_avg = [np.mean(arr) for arr in a_split]

X1= [1,100,1024,8192,32768,65536,131072]
plt.errorbar(X1,a_split_avg,yerr=errors)
plt.xlabel("Data Size (KB)")
plt.ylabel("Time (sec)")
plt.xscale("log")
plt.title("MPI Data Transfer time")
plt.show(block=True)