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


if __name__ == "__main__":
    N_MAX = 1024
    N = list(range(1, N_MAX + 1))
    I = list(map(calc_by_definitions, N))
    data = {'N': N, 'Iterations': I}
    df = pd.DataFrame(data=data)
    df.set_index(['N'], inplace=True)
    ax = df.plot.bar(y='Iterations', rot=0)
    plt.gca().set_xticks([])
    plt.xticks([])
    ax.set_xticks([])
    plt.show()
    df.to_csv('iterations_def.csv')
