import time
import sys

start = time.time()
file = open('Benchmark/Allocation/Blob.txt', "r")
blob = file.read() * 1_000
blob_len = len(blob)

i = -1
while i < blob_len-1:
	i += 1
	print('\x1B[0G', i, ' / ', blob_len, end='')

print(f"\nDone in {time.time()-start}s.")
