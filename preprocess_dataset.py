import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
#%matplotlib inline

from sklearn.model_selection import train_test_split

pd.set_option('display.max_rows', None)
pd.set_option('display.max_columns', None)
pd.set_option('display.width', 2000)
pd.set_option('display.float_format', '{:20,.2f}'.format)
pd.set_option('display.max_colwidth', None)

# RAM min percentage of variation (not included)
MIN_PERCENTAGE = 1

def balance_dataset(X, y):    
    x_df = pd.DataFrame(data=X)
    y_df = pd.DataFrame(data=y, columns=list('abcde'))    

    result = pd.concat([x_df, y_df], axis=1)   
    countClicks(result) 

    nomove  = result[(result.a == 0) & (result.b == 0) & (result.c == 0) & (result.d == 0) & (result.e == 0)].copy()
    move    = result[(result.a == 1) | (result.b == 1) | (result.c == 1) | (result.d == 1) | (result.e == 1)].copy()

    nomove = nomove[:int(nomove.shape[0]/4)].copy()
    result = pd.concat([nomove, move])    
    
    countClicks(result)    
    heatmap = result.corr(method ='pearson') 
    heatmap.style.background_gradient(cmap='Blues')
    sns.heatmap(heatmap, cmap='Blues',
            xticklabels=heatmap.columns.values,
            yticklabels=heatmap.columns.values)
    plt.show()

    X_res = result.iloc[:, :-5]    
    y_res = result.iloc[:, -5:]

    return X_res, y_res    


def generateRamIndexes(X, minPercentage):
    # heatMap = np.zeros(X[0].shape[0])
    
    # for i in range(X.shape[0]):
    #     for j in range(heatMap.shape[0]):
    #         if X[i, j] != X[0][j]:
    #             heatMap[j] += (100.0 / float(X.shape[0]))    

    # indexes = list()
    # for i in range(0, heatMap.shape[0], 1):
    #     if heatMap[i] > minPercentage:
    #         indexes.append(i)
    heatMap = []
    df = pd.DataFrame(data=X)
    for column in df.columns[:]:
        heatMap.append(df[column].nunique())

    print('Unique count: ', heatMap)

    indexes = list()
    for i in range(len(heatMap)):
        if (heatMap[i]) > minPercentage:
            indexes.append(i)

    indexes = np.asarray(indexes)
    np.savetxt(f"ramindexes.txt", indexes, fmt='%i')

    return indexes

def parseOutputTo1KeyAction(y):
    y_res = list()

    #
    # 0 - UP
    # 1 - SPACE
    # 2 - LEFT
    # 3 - RIGHT
    #
    for i in range(y.shape[0]):
        up          = y[i, 0]

        # SPACE and LEFT
        spaceLeft   = 1 if (y[i, 1] == 1 and y[i, 2] == 1) else 0
        # SPACE and RIGHT
        spaceRight  = 1 if (y[i, 1] == 1 and y[i, 3] == 1) else 0

        # not SPACE and LEFT
        left        = 1 if (y[i, 1] == 0 and y[i, 2] == 1) else 0
        # not SPACE and RIGHT
        right       = 1 if (y[i, 1] == 0 and y[i, 3] == 1) else 0
        
        y_res.append( np.array([up, spaceLeft, spaceRight, left, right]) )

    y_res = np.asarray(y_res)
    return y_res

def countClicks(y):
    nomove      = y[(y.a == 0) & (y.b == 0) & (y.c == 0) & (y.d == 0) & (y.e == 0)].shape[0]
    up          = y[(y.a == 1)].shape[0]
    spaceLeft   = y[(y.b == 1)].shape[0]
    spaceRight  = y[(y.c == 1)].shape[0]
    left        = y[(y.d == 1)].shape[0]
    right       = y[(y.e == 1)].shape[0]

    print('(NOMOVE, \tUP, \tSPACE+LEFT, \tSPACE+RIGHT, \tLEFT, \tRIGHT)') 
    print(f'(\t{nomove}, \t{up}, \t{spaceLeft}, \t{spaceRight}, \t{left}, \t{right})') 

def removeUnusedIndexes(X, indexes):
    if indexes.shape[0] == 0:
        return X

    X_res = list()    

    # Iterates through the dataset examples    
    for i in range(0, X.shape[0], 1):
        x = np.take(X[i], indexes)
        X_res.append(x)        
    
    X_res = np.asarray(X_res)
    return X_res

def save_dataset(X, y, label):
    X.to_csv(f'X_{label}.csv', index=False, header=False)
    y.to_csv(f'y_{label}.csv', index=False, header=False)

def print_results(X_train, y_train, X_valid, y_valid, X_test, y_test, indexes):
    print(f'X_train: {X_train.shape}, y_train: {y_train.shape}')
    print(f'X_valid: {X_valid.shape}, y_valid: {y_valid.shape}')
    print(f'X_test: {X_test.shape}, y_test: {y_test.shape}')
    print(f'Indexes shape: {indexes.shape}')
    print(f'Used indexes: {indexes}')

def main():        
    # Load the whole dataset X, y
    X = np.genfromtxt('x.csv', delimiter=',', dtype=int)
    y = np.genfromtxt('y.csv', delimiter=',', dtype=int)
    
    # Generate the indexes that must be taken into account when training the Net
    indexes = generateRamIndexes(X, MIN_PERCENTAGE)

    # Remove unused indexes from dataset (doesn't modify the original files)
    X = removeUnusedIndexes(X, indexes)
    y = parseOutputTo1KeyAction(y)

    X, y = balance_dataset(X, y)

    # split dataset into train and test subsets
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.1, random_state=517)
    X_train, X_valid, y_train, y_valid = train_test_split(X_train, y_train, test_size=0.2, random_state=517)

    # Save dataset
    save_dataset(X_train, y_train, 'train')
    save_dataset(X_valid, y_valid, 'valid')
    save_dataset(X_test, y_test, 'test')

    print_results(X_train, y_train, X_valid, y_valid, X_test, y_test, indexes)

main()