# AdaBoost
- Simple implementation of AdaBoost.
- This program performes binary classification using AdaBoost.

# How to use
- compile by `$make`
- prepare a data file
- run the program by `$./adaboost`
 
# Expected data file format
- tab spaced data file
- first column: target value
- second to the last columns: feature vectors
 
# Example
`./adaboost -f <file name> -n <Num. of Classifiers> -d <Dim. of the data>, [-o <out DIR>]`
where, 
- `<file name>` : input file
- `<Num of Classifiers>` : Number of Classifiers 
- `<Dim. of the data>` : Dimension of the data
- `<out DIR>` : (optional) output directory

```
$ ./adaboost -f sampledata.tsv -n3 -d4
input file:    sampledata.tsv
n: 3
d: 4
output directory is not specified
results will be written to stderr
pred	y	x
1	1	1	0	1	1	
1	1	1	0	1	1	
1	1	1	1	1	1	
0	0	1	1	1	0	
0	0	1	0	1	0	
0	0	1	1	0	1	
0	0	1	0	0	1	
0	0	1	1	0	1
```
