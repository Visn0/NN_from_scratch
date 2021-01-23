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

def removeUnusedIndexes(X, indexes):
    X_res = list()    

    # Iterates through the dataset examples    
    for i in range(0, X.shape[0], 1):
        x = np.take(X[i], indexes)  
        
        # Extracts only the necessary indexes of the example X[i]
        # for index in indexes:
        #     x.append( X[i:index] )
        
        X_res.append(x)
        print(f'Finished example: {i}\t Shape: {x.shape}')
    
    X_res = np.asarray(X_res)
    return X_res

def save_dataset(X, y, label):
    np.savetxt(f"X_{label}.csv", X, delimiter=",")
    np.savetxt(f"y_{label}.csv", y, delimiter=",")

def main():
    # Load the indexes that must be taken into account when training the Net
    indexes = np.genfromtxt('ramindexes.txt', dtype=int)
    
    # Load the whole dataset X, y
    X = np.genfromtxt('x.csv', delimiter=',')
    print(f'X shape: {X.shape}')

    y = np.genfromtxt('y.csv', delimiter=',')
    print(f'y shape: {y.shape}')
    
    # Remove unused indexes from dataset (doesn't modify the original files)
    X = removeUnusedIndexes(X, indexes)

    # split dataset into train and test subsets
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=517)

    # Save dataset
    save_dataset(X_train, y_train, 'train')
    save_dataset(X_test, y_test, 'test')

main()