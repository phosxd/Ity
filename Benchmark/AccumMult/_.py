import time
import sys

n = 30
start = time.time()

x = 'abc'
for i in range(n):
	x *= 2;
	print('\x1B[0G', (i+1), ' / ', n, end='')

print(f"\nStrings done in {time.time()-start}s.")


n = 15
start = time.time()

x = [1]
for i in range(n):
	x *= 2;
	print('\x1B[0G', (i+1), ' / ', n, end='')

print(f"\nArrays done in {time.time()-start}s.")
