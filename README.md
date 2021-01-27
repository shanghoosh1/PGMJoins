# PGMjoins
Joins big tables in relational databases efficiently

# To compile:
First cd into the folder "PGMjoins-master" then run “make -f Makefile CONF=Release”
# The Data:
You can generate the TPC-H and TP-DS data by following the instructions provided in http://www.tpc.org/   
For twitter data, send an email to us or refer to the cited related papers in our paper.  
For the JOB data and queries refer to https://github.com/gregrahn/join-order-benchmark.  
Note, all the data files should be in .csv format including the headers as attribute names, and the deliminator should be '|'. To make it easier, we provided all needed headers in Header.txt file. Note, to run our pre-defined queries in the code, you should use the same headers.  
# To run:
CD into “PGMjoins-master/dist/Release/GNU-Linux” then run “./joinpgm” with the appropriate arguments.
“./joinpgm –-help” will help you with the arguments. 
# An example: 
The following command will run the cyclic QY on TPC-H data and store the sample and running times in the output address. Note that the addresses are valid in your case.

./joinpgm -input /data/tpch/10x/ -output /results/tpch10x/ -query qyyy -algorithm 1 -sample 1000000  
Note, you should use algorithm 1 for cyclic queries , and for acyclic queries algorithm 0.

