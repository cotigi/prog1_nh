import numpy as np

period = 2*np.pi
increment = period/4096

x = np.arange(0, period, increment)
y = np.sin(x)

with open("sine.txt", "w") as f:
    for i in range(len(y)):
        f.write(f"{y[i]}\n")
