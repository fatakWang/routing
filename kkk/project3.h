//////////////////////////////////////////////////////////////////////
// Programming assignment 3: 
//
//  This is the include file shared by the simulator and by the
//  student solutions.
//
//  1.1  12/01/2013   JB    - Created for the purpose of better code?
//////////////////////////////////////////////////////////////////////

//  In this assignment, we are NOT enabling a mode that causes
//  the cost of edges to change with time.
#define LINKCHANGES 0 

//  This is the maximum number of nodes we will ever have.  We MAY not 
//  actually implement this many, but the structures allow for it.
#define  MAX_NODES    4

struct NeighborCosts   {
    int  NodesInNetwork;
    int  NodeCosts[MAX_NODES];
};

// a RoutePacket is the packet sent from one routing update process to
// another via the call tolayer3()

struct RoutePacket {
   int sourceid;               // id of sending router sending this pkt
   int destid;                 // id of router to which pkt being sent 
                               // (must be an immediate neighbor) 
   int mincost[MAX_NODES];     // min cost from node 0 ... N 
};

#define   YES           1
#define   NO            0
#define   INFINITY   9999

#define   MAX_FILENAME_LENGTH  80



// Prototype so all nodes can see the routine in project3.c
void                  toLayer2( struct RoutePacket packet );
struct NeighborCosts *getNeighborCosts(int myNodeNumber);
