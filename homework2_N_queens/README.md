### N-Queens

For this assignment I used 2 different approaches for experimental purposes:

- In **homework2.cpp** I put the queens on the chess board randomly in a way that each queen is alone in her row and column. After that the algorithm minimizes the number of conflicts. **_On my machine the program finds a solution for 10000 queens for 0.378455 seconds._**

- In **homework2_min.cpp** I put the queens on the chess board in the following way:
  1. the i-th queen is put in the i-th column
  2. for each cell in the i-th column compute the number of conflicts with the already placed queens on the board (from queen 1 to queen i-1) and put the i-th queen in the cell where this number is minimized. After that the algorithm minimizes the number of conflicts (which are expected to be less initially due to the smarter inicialization step). **_On my machine the program finds a solution for 10000 queens for 0.618263 seconds._**

**Remark**: the program prints the solution up to N = 100 queens. If N > 100 the output of the program is only the execution time
