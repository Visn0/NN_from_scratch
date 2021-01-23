import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split

# df = pd.read_csv('x_train.csv', sep=',', decimal='.')
# pd.set_option('display.max_rows', None)
# pd.set_option('display.max_columns', None)
# pd.set_option('display.width', 2000)
# pd.set_option('display.float_format', '{:20,.2f}'.format)
# pd.set_option('display.max_colwidth', None)
# print(df.describe(include='all').T)

# RAM min percentage of variation (not included)
MIN_PERCENTAGE = 0.0

def printArrayAsTable(rows, cols, array):
    index = 0
    for i in range(0, rows, 1):
        for j in range(0, cols, 1):
            print(f'{array[index]}', sep=',')
            index += 1

def generateRamIndexes(X, minPercentage):
    heatMap = np.zeros(X[0].shape[0])
    
    for i in range(X.shape[0]):
        for j in range(heatMap.shape[0]):
            if X[i, j] != X[0][j]:
                heatMap[j] += (100.0 / float(X.shape[0]))

    # printArrayAsTable(8, 16, heatMap)

    indexes = list()
    for i in range(0, heatMap.shape[0], 1):
        if heatMap[i] > minPercentage:
            indexes.append(i)

    indexes = np.asarray(indexes)
    np.savetxt(f"ramindexes.txt", indexes, fmt='%i')
    print(f'\nChanging indexes: {indexes}')

    return indexes
    

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
    np.savetxt(f"X_{label}.csv", X, delimiter=",", fmt='%i')
    np.savetxt(f"y_{label}.csv", y, delimiter=",", fmt='%i')

def main():        
    # Load the whole dataset X, y
    X = np.genfromtxt('x.csv', delimiter=',', dtype=int)
    print(f'X shape: {X.shape}')

    y = np.genfromtxt('y.csv', delimiter=',', dtype=int)
    print(f'y shape: {y.shape}')
    
    # Generate the indexes that must be taken into account when training the Net
    indexes = generateRamIndexes(X, MIN_PERCENTAGE)
    print(f'Indexes shape: {indexes.shape}')

    # Remove unused indexes from dataset (doesn't modify the original files)
    X = removeUnusedIndexes(X, indexes)

    # split dataset into train and test subsets
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=517)

    # Save dataset
    save_dataset(X_train, y_train, 'train')
    save_dataset(X_test, y_test, 'test')

main()