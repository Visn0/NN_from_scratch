
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
import matplotlib.ticker as ticker
import numpy as np
import bisect


def cplot(df):
    print(df.shape)    
    fig, ax = plt.subplots()    
    ax.plot(df[:, 0], color='blue', label='train')
    ax.plot(df[:, 1], color='orange', label='test')

    ax.set(xlabel='epoch', ylabel='error',
        title='Mean squared error')
    ax.grid()

    ax.legend()
    fig.savefig("test.png")
    plt.show()

df = np.genfromtxt('history.csv', delimiter=',')

print(df)
cplot(df)
