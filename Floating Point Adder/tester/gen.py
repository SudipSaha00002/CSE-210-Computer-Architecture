import random

from adder import *

a = '00000000101100000000000000000011'
b = '10000001001010000000000000000010'
ans, overflow, underflow = add_floating_points(a, b)
print(a)
print(b)
print(ans, overflow, underflow)
