README

A failure detector using a gossip-based heartbeat algorithm of nodes (synonymous with a process in the following). It takes following command line  parameters:

N: number of peer nodes,
b: gossip parameter(number of neighbouring nodes),
c: gossip parameter(number of iterations),
F: number of seconds after which a node is considered dead,
B: number of bad nodes that should fail,
P: number of seconds to wait between failures.
S: The seed of the random number generator.
T: Total number of seconds to run


Compiling the program

It can be done by using "make" command. You can clean the files using "make clean" which will clean all the executables and objects.
It also removes the files which is created after running the program which is "endpoints.txt" and "list*.txt".

Correctness: 

This was tested with five nodes because value of N is 5. We used 2 AW machines which is AW01 and AW09.
---------------------------------------------------------------------------------------------------------
Test Case 1: ./p4 5 2 50 5 2 10 5 50

This will do 50 iterations and since number of seconds to wait between failures is 10, nodes will start failing every 10 seconds.
It will run for 50 seconds and the program will exit.

Once a node fails itself heart beat of the node will be consistent across. It will stop sending data to other nodes.

For the above program, the results were like this:
2 nodes failed in 10 and 20th seconds.
Other 3 nodes did not fail, and the output was consistent across all the nodes. 

---------------------------------------------------------------------------------------------------------
Test Case 2: ./p4 5 2 50 5 0 10 5 50

This will do 50 iterations and since the number of nodes to fail is 0, none of the nodes will fail themselves.

For the above program, the results were like this:
All the 5 nodes were alive as none of the nodes failed, and the output was consistent across all the nodes. 

---------------------------------------------------------------------------------------------------------
Test Case 3: ./p4 5 2 100 5 5 10 5 100

This will do 100 iterations and since number of seconds to wait between failures is 10, nodes will start failing every 10 seconds.
It will run for 100 seconds and the program will exit. However, when the last node fails, every other node has also stopped sending messages
and this failure wont be detected because no node will now send any message

For the above program, the results were like this:
All the 5 nodes failed in 10, 20, 30, 40 and 50th seconds, but the output was not consistent across all the nodes 
because the node which failed at t=50 is not yet recognized by the other nodes as there were no messages passed between the nodes after the last failure
---------------------------------------------------------------------------------------------------------