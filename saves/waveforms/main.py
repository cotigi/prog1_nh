import numpy as np

period = 2*np.pi
sample_count = 4096

x = np.linspace(0, period, sample_count, endpoint=False)

y = 1 - (x / np.pi)

with open("falling-saw.txt", "w") as f:
    for i in range(len(y)):
        f.write(f"{y[i]}\n")
