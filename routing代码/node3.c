#include <stdio.h>
#include <stdlib.h>
#include "project3.h"

#define NODEID 3

extern int TraceLevel;
extern float clocktime;

struct distance_table {
  int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt3;
struct NeighborCosts   *neighbor3;
int numNodes;

void printdt3( int MyNodeNumber, struct NeighborCosts *neighbor, 
		struct distance_table *dtptr );

// returns the minimum value in an array of count integers
int min3(int count, int array[]) {
	int minimum;

	minimum = array[--count];

	while (count >= 0) {
		if (array[count] < minimum) minimum = array[count];
		count--;
	}

	return minimum;
}


/* students to write the following two routines, and maybe some others */

/* rtinit3()
 * This routine is called once at the beginning of the emulation. It takes no
 * arguments. It should initialize the distance table in node0 to reflect the
 * direct costs to its neighbors by using GetNeighborCosts(). It should also
 * send to its neighbors the minimum cost paths to all other network nodes in a
 * routing packet using toLayer2().
 */
static int min_vertic[MAX_NODES];

void rtinit3() {
	int i, j;
	struct RoutePacket toSend;

	if (TraceLevel >= 2) {
		printf("At time t=%.3f,rtinid%d() called.\n", clocktime, NODEID);
	}

	// get the initial neighbors
	neighbor3 = getNeighborCosts(NODEID);
	numNodes = neighbor3->NodesInNetwork;

	// initialize distance table
	for (i = 0; i < numNodes; i++) {
		for (j = 0; j < numNodes; j++) {
			if (i == j) { // directly connected neighbor
				dt3.costs[i][j] = neighbor3->NodeCosts[i];
				min_vertic[i]=neighbor3->NodeCosts[i];
			}
			else {
				dt3.costs[i][j] = INFINITY;
			}
		}
	}

	// printf("\n---min_vertic--\n");
	// for(int i=0;i<4;i++){
	// 	printf("   %d",min_vertic[i]);
	// }
	// printf("\n");
	printf("node%d initial distance table:\n", NODEID);
	printdt3(NODEID, getNeighborCosts(NODEID), &dt3);
	
	if (TraceLevel >= 2) {
		printf("constructing packets to send...\n");
	}

	// construct packets to send to neighbors
	for (i = 0; i < numNodes; i++) {
		if (i != NODEID && neighbor3->NodeCosts[i] < INFINITY) {
			if (TraceLevel >= 3) printf("making packet %d...\n", i);
			toSend.sourceid = NODEID;
			toSend.destid = i;
			for (j = 0; j < numNodes; j++) {
				toSend.mincost[j] = dt3.costs[j][j];
			}
			// send packet to neighbor
			toLayer2(toSend);
		}
	}
}

/* rtupdate3(struct RoutePacket *rcvdpkt)
 * Called when this node receives a routing packet that was sent to it by one of
 * its directly connected neighbors. The parameter *recvdpkt is a pointer to the
 * packet that was received. rtupdate3() is the heart of the distance vector
 * algorithm. The values it receives in a routing pcket from some other node i
 * contains i's current shortest path costs to all other network nodes.
 * rtupdate3() uses these received values to update its own distance table. If
 * its own minimum cost to another node changes as a result of the update, this
 * node informs its directly connected neighbors of this change by sending them
 * a routing packet.
 */


void rtupdate3( struct RoutePacket *rcvdpkt ) {
	int i, j, n, tableUpdated, src,costupdated;
	struct RoutePacket toSend;

	if (TraceLevel >= 2) {
		printf("At time t=%.3f, node%d received routing packet from %d\n"
				, clocktime, NODEID, rcvdpkt->sourceid);
	}

	// don't break 18 USC Section 1702 (reading someone else's mail)!
	if (rcvdpkt->destid != NODEID) {
		printf("node%d received %d's mail\n", NODEID, rcvdpkt->destid);
		exit(1);
	}

	costupdated=tableUpdated = 0; // flag
	src = rcvdpkt->sourceid;

	for (i = 0; i < numNodes; i++) {
		if (dt3.costs[i][src] > dt3.costs[src][src] + rcvdpkt->mincost[i]) {
			dt3.costs[i][src] = dt3.costs[src][src] + rcvdpkt->mincost[i];
			tableUpdated=1;
		}
	}
	if(tableUpdated){
		printf("node%d distance table updated: \n", NODEID);
		printdt3(NODEID, neighbor3, &dt3);
	}

	// printf("\n--- dt3 ----\n");
	for (i = 0; i < numNodes; i++) {
		for (j = 0; j < numNodes; j++) {
			// printf("   %d",dt3.costs[i][j]);
			if (min_vertic[i] > dt3.costs[i][j]){
				min_vertic[i] = dt3.costs[i][j];
				costupdated=1;
			}
		}
		// printf("\n");
	}



	// printf("\ntoSend---\n");
	for (i = 0; i < numNodes; i++) {
		toSend.mincost[i] = min_vertic[i];
		// printf("   %d",min_vertic[i]);
	}
// printf("\n");
	if (costupdated) {
		

		// initialize toSend
		
		// construct packets to send to neighbors
		toSend.sourceid = NODEID;
		for (n = 0; n < numNodes ; n++) {//
			if(n==NODEID||neighbor3->NodeCosts[n]==INFINITY)continue;
			toSend.destid = n;
			if (TraceLevel >= 2) {
				printf("At time t=%.3f, node %d sends packet to node %d with: ", clocktime, NODEID, n);
				for (i = 0; i < numNodes; i++) {
					printf("%d ", toSend.mincost[i]);
				}
				printf("\n");
			}
			toLayer2(toSend);
		}

	}

}


/////////////////////////////////////////////////////////////////////
//  printdt
//  This routine is being supplied to you.  It is the same code in
//  each node and is tailored based on the input arguments.
//  Required arguments:
//  MyNodeNumber:  This routine assumes that you know your node
//                 number and supply it when making this call.
//  struct NeighborCosts *neighbor:  A pointer to the structure 
//                 that's supplied via a call to getNeighborCosts().
//                 It tells this print routine the configuration
//                 of nodes surrounding the node we're working on.
//  struct distance_table *dtptr: This is the running record of the
//                 current costs as seen by this node.  It is 
//                 constantly updated as the node gets new
//                 messages from other nodes.
/////////////////////////////////////////////////////////////////////
void printdt3( int MyNodeNumber, struct NeighborCosts *neighbor, 
		struct distance_table *dtptr ) {
    int i, j;
    int TotalNodes = neighbor->NodesInNetwork;     // Total nodes in network
    int NumberOfNeighbors = 0;                     // How many neighbors
    int Neighbors[MAX_NODES];                      // Who are the neighbors

    // Determine our neighbors 
    for ( i = 0; i < TotalNodes; i++ )  {
        if (( neighbor->NodeCosts[i] != INFINITY ) && i != MyNodeNumber )  {
            Neighbors[NumberOfNeighbors] = i;
            NumberOfNeighbors++;
        }
    }
    // Print the header
    printf("                via     \n");
    printf("   D%d |", MyNodeNumber );
    for ( i = 0; i < NumberOfNeighbors; i++ )
        printf("     %d", Neighbors[i]);
    printf("\n");
    printf("  ----|-------------------------------\n");

    // For each node, print the cost by travelling thru each of our neighbors
    for ( i = 0; i < TotalNodes; i++ )   {
        if ( i != MyNodeNumber )  {
            printf("dest %d|", i );
            for ( j = 0; j < NumberOfNeighbors; j++ )  {
                    printf( "  %4d", dtptr->costs[i][Neighbors[j]] );
            }
            printf("\n");
        }
    }
    printf("\n");
}    // End of printdt3

