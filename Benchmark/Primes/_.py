import time


def iSqrt(n):
    x = n
    y = ((x+1) / 2)
    while y < x:
        x = y
        y = ( ((n/x) + x) / 2)
    return int(x)


def is_prime(n):
    if n%2 == 0: return n == 2
    r = iSqrt(n)
    for i in range(3, r+1, 2):
        if n%i == 0:
            return False
    return True


n = int(input("Count: "))
start = time.time()


for p in range(2, n+1):
    if is_prime(p):
        print(p)


print(f"\nDone in {time.time()-start}s.")

