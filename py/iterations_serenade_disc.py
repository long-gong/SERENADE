import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


# def calc_by_understanding(n):
#     """Calculate # of iterations when cycle length is n"""
#     visited = [0] * n
#     visited[0] = 1
#     it = 0
#     for k in range(n + 1):
#         it += 0.5
#         i = ((2 ** k) % n)
#         if visited[i] == 1:
#             break
#         else:
#             visited[i] = 1
#     return int(np.ceil(it))
def factorize(n):
    f = []
    for a in range(1, n+1):
        if (n % a) == 0:
            f.append(a)
    return f


def ouroboros(n):
    """Calculate ouroboros numbers"""
    logn = int(np.ceil(np.log2(n)))
    dlist = [1]*n
    for i in range(logn + 1):
        for a in factorize(2**i):
            if a <= n:
                dlist[a-1] = 0
    for beta in range(logn + 1):
        for gamma in range(logn + 1):
            for a in factorize((2**beta) + (2**gamma)):
                if a <= n:
                    dlist[a-1] = 0
            if beta > gamma:
                for a in factorize((2**beta) - (2**gamma)):
                    if a <= n:
                        dlist[a-1] = 0
    return dlist


def create_sigma(n):
    """Create a combinatorial cycle with size of n"""
    sigma = [0] * n
    sigma_inv = [0] * n
    for k in range(n):
        ki = ((k + 1) % n)
        sigma[k] = ki
        sigma_inv[ki] = k
    return sigma, sigma_inv


def calc_by_definitions(n):
    """Calculate # of iterations when cycle length is n by definitions"""
    sigma, sigma_inv = create_sigma(n)
    visited = [0] * n
    visited[0] = 1
    it = 0
    for k in range(n):
        it += 1
        l = ((2 ** k) % n)
        a = 0
        for i in range(l):
            a = sigma[a]
        b = 0
        for i in range(l):
            b = sigma_inv[b]
        if visited[a] == 1 or visited[b] == 1 or a == b:
            break
        else:
            visited[a] = 1
            visited[b] = 1
    return it


def cross_verify():
    for n in [8, 16, 32, 64, 128, 256, 512, 1024]:
        with open('./ouroboros/n={}.txt'.format(n), 'r') as fp1:
            lines = fp1.read().splitlines()
            d1 = [ int(d) for d in lines ]
            with open('../data/n={}.txt'.format(n), 'r') as fp2:
                lines = fp2.read().splitlines()
                d2 = [ int(d) for d in lines ]
            for a,b in zip(d1, d2):
                if a != b :
                    print ("{n} {a} {b}".format(n=n,a=a,b=b))


if __name__ == "__main__":
    # N_MAX = 1024
    # N = list(range(1, N_MAX + 1))
    # I = list(map(calc_by_definitions, N))
    # data = {'N': N, 'Iterations': I}
    # df = pd.DataFrame(data=data)
    # df.set_index(['N'], inplace=True)
    # ax = df.plot.bar(y='Iterations', rot=0)
    # plt.gca().set_xticks([])
    # plt.xticks([])
    # ax.set_xticks([])
    # plt.show()
    # df.to_csv('iterations_def.csv')

    # for n in [8, 16, 32, 64, 128, 256, 512, 1024]:
    #     dlist = ouroboros(n)
    #     with open('ouroboros/n={}.txt'.format(n), 'w') as f:
    #         for d in dlist:
    #             f.write('{}\n'.format(d))
                # f.write('\n')
    cross_verify()


