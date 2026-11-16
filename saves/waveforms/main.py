import numpy as np

period = 2*np.pi
increment = period/4096
MAX = 32768

x = np.arange(0, period, increment)
y = np.sin(x)

for i in range(len(y)):
    y[i] = int(y[i]*MAX)

with open("sine.txt", "w") as f:
    for i in range(len(y)):
        f.write(f"{y[i]}\n")
