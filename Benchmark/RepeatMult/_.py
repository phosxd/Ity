import time
import sys

n = 200_000
start = time.time()

for i in range(n):
	x = 'abc' * 250;
	print('\x1B[0G', i, ' / ', n, end='')

print(f"\nDone in {time.time()-start}s.")
