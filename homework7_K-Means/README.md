### K-Means and K-Means++

**About the datasets**:

- we have 2 datasets - **normal.txt** and **unbalance.txt**. We will try to cluster the data in each dataset with K-Means and K-Means++ algorithms

**Input to the C++ program**:

- the user enters "U" and "K" -> K-Means algorithm is executed on **unbalance.txt** and the output is stored in **unbalance_output_K-Means.txt**
- the user enters "U" and "K++" -> K-Means++ algorithm is executed on **unbalance.txt** and the output is stored in **unbalance_output_K-Means++.txt**
- the user enters "N" and "K" -> K-Means algorithm is executed on **normal.txt** and the output is stored in **normal_output_K-Means.txt**
- the user enters "N" and "K++" -> K-Means++ algorithm is executed on **normal.txt** and the outut is stored in **normal_output_K-Means++.txt**

_After the output of the C++ program is generated, the clusters can be visualized with the Python script_

**Input to the Python script**:

- the user executes **visualize_data** function with "U" and "K" -> visualization of **unbalance_output_K-Means.txt** is stored as **unbalance_output_K-Means.png**

- the user executes **visualize_data** function with "U" and "K++" -> visualization of **unbalance_output_K-Means++.txt** is stored as **unbalance_output_K-Means++.png**

- the user executes **visualize_data** function with "N" and "K" -> visualization of **normal_output_K-Means.txt** is stored as **normal_output_K-Means.png**

- the user executes **visualize_data** function with "N" and "K++" -> visualization of **normal_output_K-Means++.txt** is stored as **normal_output_K-Means++.png**

**Visualizations**: open the generated **.png** file and explore how the algorithms clustered the data. K-Means or K-Means++ did a better job?
