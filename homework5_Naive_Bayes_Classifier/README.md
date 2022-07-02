### Naive Bayes Classifier

**About the dataset**:
* Naive Bayes Classifier that is trained to predict if a person is democrat or repubican based on 16 input features
* you can explore and gain more information about the [dataset](http://archive.ics.uci.edu/ml/datasets/Congressional+Voting+Records)

**How it works**:
* the dataset is shuffled randomly
* the dataset is partitioned into 10 roughly equal in number of examples chunks (from **chunk number 1** to **chunk number 10**)
* **Training and measuring accuracy**:
    1. the model is created and trained on the data **from chunk 2 to chunk 10 inclusively**
    2. the model is tested with data it has never seen before, i.e. it is tested with **chunk 1**
    3. the accuracy of the model is calculated based on how many correct predictions it made and that accuracy is stored

    4. the model is created again and trained on the data from **chunk 1** and the data **from chunk 3 to chunk 10 inclusively**
    5. the model is tested with data it has never seen before, i.e. it is tested with **chunk 2**
    6. the accuracy of the model is calculated based on how many correct predictions it made and that accuracy is stored

    * Analogously this procedure continues (the model is trained on 9 of the chunks and tested on the remaining chunk) until the model is tested on all 10 chunks
    * After that the accuracy of the model is computed as mean of the sum of the stored accuracies

**Data Preprocessing**:
* some of the examples have "?"
* the function **solution_with_original_values** works with the original data, i.e. works with "?" in some places. ***The accuracy I obtain on my machine is: 89.8837%***
* the function **solution_with_only_yes_no** works with preprocessed data. i.e. every "?" is replaced with the dominating answer from the corresponding class (democrat or republican), i.e. if an example of class republican has "?" for some feature then this "?" is replaced with the majority answer from all republicans that have answer different from "?" for this feature. ***The accuracy I obtain on my machine is: 91.723%***

**Remark**: in single execution of the program call either of the functions **solution_with_original_values** and **solution_with_only_yes_no** in main, but not both of them simultaneously
