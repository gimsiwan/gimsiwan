import numpy as np
import math

M = np.arange(2,27,1)
print(M)
print()

M = M.reshape(5,5)
print(M)
print()

M[1:4,1:4] = 0
print(M)
print()

M = M @ M
print(M)
print()

v = M[0]
v = v@v
print(math.sqrt(v))
print()

