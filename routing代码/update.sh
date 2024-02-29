#! /bin/bash

# By Joseph Petitti

# This script copies node0.c to node1.c, node2.c, and node3.c replacing any
# functions that are specific to node0 with their equivalents. This is necessary
# because the code I was given in the assignment (and that I'm not allowed to
# change) requires huge amounts of repeated code.

cp node0.c node1.c;
sed -i 's/min0/min1/g' node1.c;
sed -i 's/rtupdate0/rtupdate1/g' node1.c;
sed -i 's/rtinit0/rtinit1/g' node1.c
sed -i 's/printdt0/printdt1/g' node1.c;
sed -i 's/neighbor0/neighbor1/g' node1.c;
sed -i 's/dt0/dt1/g' node1.c;
sed -i 's/NODEID 0/NODEID 1/g' node1.c;

cp node0.c node2.c;
sed -i 's/min0/min2/g' node2.c;
sed -i 's/rtupdate0/rtupdate2/g' node2.c;
sed -i 's/rtinit0/rtinit2/g' node2.c
sed -i 's/printdt0/printdt2/g' node2.c;
sed -i 's/neighbor0/neighbor2/g' node2.c;
sed -i 's/dt0/dt2/g' node2.c;
sed -i 's/NODEID 0/NODEID 2/g' node2.c;

cp node0.c node3.c;
sed -i 's/min0/min3/g' node3.c;
sed -i 's/rtupdate0/rtupdate3/g' node3.c;
sed -i 's/rtinit0/rtinit3/g' node3.c
sed -i 's/printdt0/printdt3/g' node3.c;
sed -i 's/neighbor0/neighbor3/g' node3.c;
sed -i 's/dt0/dt3/g' node3.c;
sed -i 's/NODEID 0/NODEID 3/g' node3.c;
