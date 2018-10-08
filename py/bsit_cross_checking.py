import pandas as pd


if __name__ == "__main__":
    for n in [8, 16,32 ,64, 128]:
        with open('./bsit-old/BSIT_{}.txt'.format(n), 'r') as fp1:
            df = pd.read_csv(fp1, sep=' ', dtype=int)
            # print(df.head())
            with open('../data/bsit/bsit_n={}.csv'.format(n), 'r') as fp2:
                df_prime = pd.read_csv(fp2,skiprows=1,dtype=int)
                # print(df.head())
                for a,b in zip(df.iloc[:, 1], df_prime.iloc[:, 1]):
                    if a != b:
                        print('error n = {}'.format(n))
                    else:
                        print("{a} {b}".format(a=a,b=b))
