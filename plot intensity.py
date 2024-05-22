import numpy as np
import matplotlib.pyplot as plt

def read_matrix_from_file(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()
        matrix = []
        for line in lines:
            if line.strip():  # Skip any empty lines
                row = list(map(float, line.split()))
                matrix.append(row)
        return np.array(matrix)

def plot_matrix(matrix):
    plt.imshow(matrix, cmap='viridis', interpolation='nearest')
    plt.colorbar()
    plt.title('2D Matrix Visualization')
    plt.xlabel('Column')
    plt.ylabel('Row')
    #print(np.shape(matrix[0][10]))
    #plt.plot(matrix[200])
    plt.show()

def main():
    filename = 'RawData/phase40.txt'  # Replace with your file name
    matrix = read_matrix_from_file(filename)
    plot_matrix(matrix)

if __name__ == "__main__":
    main()

