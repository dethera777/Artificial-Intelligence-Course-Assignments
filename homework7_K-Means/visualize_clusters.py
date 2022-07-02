import numpy as np
import matplotlib.pyplot as plt

# this function takes the output of the C++ program (the output files) and visualizes the clusters
def visualize_data(data_file_name, algorithm_name):
    output_file_name_normal_k_means = "normal_output_K-Means.txt"
    output_file_name_normal_k_means_plus_plus = "normal_output_K-Means++.txt"
    output_file_name_unbalance_k_means = "unbalance_output_K-Means.txt"
    output_file_name_unbalance_k_means_plus_plus = "unbalance_output_K-Means++.txt"
    
    if data_file_name == "U" and algorithm_name == "K":
        data_file_name = output_file_name_unbalance_k_means
    elif data_file_name == "U" and algorithm_name == "K++":
        data_file_name = output_file_name_unbalance_k_means_plus_plus
    elif data_file_name == "N" and algorithm_name == "K":
        data_file_name = output_file_name_normal_k_means
    elif data_file_name == "N" and algorithm_name == "K++":
        data_file_name = output_file_name_normal_k_means_plus_plus
    else:
        print("Invalid input!!!")
        return

    xs = np.array([])
    ys = np.array([])
    colors = np.array([])

    with open(data_file_name, 'r') as f:
        for line in f:
            for index, value in enumerate(line.split()):
                if (index == 0):
                    x1 = float(value)
                    xs = np.append(xs, x1)
                elif (index == 1):
                    x2 = float(value)
                    ys = np.append(ys, x2)
                else:
                    col = int(value)
                    colors = np.append(colors, col)

    plt.scatter(xs, ys, c = colors)
    image_file_name = (data_file_name[:-3] + "png")
    plt.savefig(image_file_name)

#visualize_data("U", "K")
#visualize_data("U", "K++")
#visualize_data("N", "K")
#visualize_data("N", "K++")