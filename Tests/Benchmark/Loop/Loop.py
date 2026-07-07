import time

n = int(input("Count: "))
start = time.time()

for i in range(n):
	print(i)

print(f"\nDone in {time.time()-start}s.")
