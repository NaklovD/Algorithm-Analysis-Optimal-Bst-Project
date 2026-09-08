================================================================
 OPTIMAL BINARY SEARCH TREE (OBST) - DYNAMIC PROGRAMMING
 Design and Analysis of Algorithms - Term Project
================================================================

FILES
-----
 obst.cpp                Source code (C++)
 input.txt               Sample input, n = 5 (the example in the assignment)
 input_n8.txt            Second test input, n = 8
 sample_output_n5.txt    Program output for input.txt
 sample_output_n8.txt    Program output for input_n8.txt
 README.txt              This file


RUN
---
  ./obst input.txt   reads that file directly (no questions asked)
  ./obst             asks whether to read a file or to type the values

INPUT FORMAT
------------
 First line : n (number of keys)
 Next lines : key  probability

 Example:
   5
   10 0.10
   20 0.20
   30 0.40
   40 0.20
   50 0.10

 The program stops with an error message if:
   - the keys are not in increasing order
   - a probability is negative
   - the probabilities do not sum to 1 (tolerance 1e-6)
   - the file is missing or a line is incomplete

OUTPUT
------
  1. Input keys and probabilities
  2. DP cost table C[i][j] and root table R[i][j]   (printed for n <= 10)
  3. Minimum expected search cost C[1][n]
  4. Structure of the optimal BST
  5. Level of each key
  6. Verification: cost from the tree vs cost from the DP table
  7. Comparison with a conventional BST
  8. Experimental analysis for n = 5, 10, 20, 50, 100

ALGORITHM
---------
 C[i][i]   = p[i]
 C[i][i-1] = 0
 C[i][j]   = min over r in [i..j] of ( C[i][r-1] + C[r+1][j] )
             + ( p[i] + ... + p[j] )
 R[i][j]   = the r that gives the minimum

 Time  : O(n^3)
 Space : O(n^2)
