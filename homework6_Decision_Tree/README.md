### Decision Tree

**About the dataset**:

- Decision tree that is trained to predict the correct class of an example described by 9 features
- you can explore and gain more information about the [dataset](https://archive.ics.uci.edu/ml/datasets/breast+cancer)

**How it works**:

- the dataset is shuffled randomly
- the dataset is partitioned into 10 roughly equal in number of examples chunks (from **chunk number 1** to **chunk number 10**)
- **Training and measuring accuracy**:

  1. the model is created and trained on the data **from chunk 2 to chunk 10 inclusively**
  2. the model is tested with data it has never seen before, i.e. it is tested with **chunk 1**
  3. the accuracy of the model is calculated based on how many correct predictions it made and that accuracy is stored
  4. the model is created again and trained on the data from **chunk 1** and the data **from chunk 3 to chunk 10 inclusively**
  5. the model is tested with data it has never seen before, i.e. it is tested with **chunk 2**
  6. the accuracy of the model is calculated based on how many correct predictions it made and that accuracy is stored

  - Analogously this procedure continues (the model is trained on 9 of the chunks and tested on the remaining chunk) until the model is tested on all 10 chunks

  - After that the accuracy of the model is computed as mean of the sum of the stored accuracies

**Details**:

- Decision trees tend to overfit to the data in the training set. To cope with this problem 2 approaches are implemented:
  - using **_pre-pruning_** while the tree is being build. If a node in the tree contains less than **_k_** (hyperparameter) examples then this node becomes a leaf. The class of the leaf is the prevailing class of the leaf's examples. **_The accuracy I obtain on my machine with k == 4 is: 73.4852%_**
  - using **_post-pruning_** after the tree is built. A **_maximum depth constant_** is set and all the nodes with greater depth are pruned. The new leaves are the nodes with **_depth == maximum depth constant_**. The class of a leaf is the prevailing class of all the examples contained in that leaf node. **_The accuracy I obtain on my machine with maximum depth constant == 3 is: 69.6798%_**

**Remark**: in single execution of the program call either of the functions **solve_pre_pruning_with_k** and **solve_post_pruning_with_max_depth** in main, but not both of them simultaneously
