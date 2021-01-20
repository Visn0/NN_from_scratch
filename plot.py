
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
import matplotlib.ticker as ticker
import numpy as np
import bisect


def cplot(df):
    print(df.shape)
    fig, ax = plt.subplots()
    ax.plot(df)

    ax.set(xlabel='epoch', ylabel='error',
        title='Mean squared error')
    ax.grid()

    fig.savefig("test.png")
    plt.show()

df = pd.read_csv('history.csv')

print(df)
cplot(df)
