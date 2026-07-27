import time
import sys

start = time.time()
file = open('Benchmark/Allocation/Blob.txt', "r")
blob = file.read() * 100_000

print(sys.getsizeof(blob))
print(f"\nDone in {time.time()-start}s.")
