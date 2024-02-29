//////////////////////////////////////////////////////////////////////
// Programming assignment 3: 
//    Implementing distributed, asynchronous, distance vector routing.
//
//  This is the simulator.  It should not be necessary for students
//  to modify this code.
//
//  1.0  Ancient      Kurose
//  1.1  12/01/2013   JB    - Lots of cleanup, comments, 
//                            and corrections
//  1.11 12/12/2013   JB    - Initialize access to node4 and node5
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project3.h"

int TraceLevel = 1;                  // Trace level - set on command line

//////////////////////////////////////////////////////////////////////
////////////////// NETWORK EMULATION CODE STARTS BELOW 
//The code below emulates the layer 2 and below network environment:
//  - emulates the tranmission and delivery (with no loss and no
//    corruption) between two physically connected nodes
//  - calls the initializations routines rtinit0, etc., once before
//    beginning emulation
//
//THERE IS NO REASON THAT ANY STUDENT SHOULD HAVE TO READ OR UNDERSTAND
//THE CODE BELOW.  YOU SHOULD NOT TOUCH, OR REFERENCE (in your code) ANY
//OF THE DATA STRUCTURES BELOW.  If you're interested in how I designed
//the emulator, you're welcome to look at the code - but again, you 
//will not modify this code.
//////////////////////////////////////////////////////////////////////

//  This is the structure for an event that's placed on the event Q
struct event {
   float evtime;                 // event time 
   int evtype;                   // event type code 
   int eventity;                 // entity where event occurs 
   struct RoutePacket *rtpktptr; // ptr to packet (if any) for this event 
   struct event *prev;           // Doubly linked list
   struct event *next;           // Doubly linked list
};
// This is the head of the event list.
struct event *evlist = NULL;   

// There are only two kinds of events we'll see here.
#define  FROM_LAYER2     2
#define  LINK_CHANGE     10

// This is the simulation time
float clocktime = 0.000;

// This data is read from the configuration file
// It includes how many nodes we actually have in this configuration
// And it has the connect costs that exist for this configuration

#define   DefaultConfiguration  "NodeConfigurationFile"

char ConfigurationFilename[MAX_FILENAME_LENGTH];
int  NumberOfNodes;                          
int  ConnectCosts[MAX_NODES][MAX_NODES];

//  Prototypes
double  GetRandomNumber();
void    insertevent( struct event *p );
void    init();
void    rtinit0(), rtinit1(), rtinit2(), rtinit3(), rtinit4();
//void    rtinit5(), rtinit6(), rtinit7(), rtinit8(), rtinit9();
void    rtupdate0( struct RoutePacket *), rtupdate1( struct RoutePacket *); 
void    rtupdate2( struct RoutePacket *), rtupdate3( struct RoutePacket *);
//void    rtupdate4( struct RoutePacket *), rtupdate5( struct RoutePacket *);
//void    rtupdate6( struct RoutePacket *), rtupdate7( struct RoutePacket *);
//void    rtupdate8( struct RoutePacket *), rtupdate9( struct RoutePacket *);
// void    linkhandler0(int , int ), linkhandler1(int , int );
// void    linkhandler2(int , int ), linkhandler3(int , int );

// The program comes here on startup.
int   main( int argc, char *argv[]) {
    struct event *eventptr;
   
    // We expect only one argument - the TraceLevel.  If we don't see it,
    // then ask.
    if (argc > 1 )
        TraceLevel = atoi( argv[1] );
    else {
        printf("Enter Trace Level: ");
        scanf("%d", &TraceLevel);
    }
    strncpy( ConfigurationFilename, DefaultConfiguration, 
             strlen(DefaultConfiguration) );

    // We have a secret switch that allows a user to enter the name of 
    // an alternate configuration file.  This should be helpful when 
    // running student projects.

    if (argc > 2 )  {
        strncpy( ConfigurationFilename, argv[2], MAX_FILENAME_LENGTH );
        printf( "Using configuration file %s\n", ConfigurationFilename );
    }

    init();
   
    // Loop forever processing the event list
    while (1) {
     
       eventptr = evlist;            // get next event to simulate 
       if (eventptr == NULL)
           goto terminate;
       evlist = evlist->next;        // remove this event from event list 
       if (evlist!=NULL)
           evlist->prev=NULL;
       if (TraceLevel>1) {
           printf("MAIN: rcv event, t=%.3f, at %d",
                          eventptr->evtime,eventptr->eventity);
           if (eventptr->evtype == FROM_LAYER2 ) {
                printf(" src:%2d,",eventptr->rtpktptr->sourceid);
                printf(" dest:%2d,",eventptr->rtpktptr->destid);
                printf(" contents: %3d %3d %3d %3d\n", 
                  eventptr->rtpktptr->mincost[0], eventptr->rtpktptr->mincost[1],
                  eventptr->rtpktptr->mincost[2], eventptr->rtpktptr->mincost[3]);
            }
        }
        clocktime = eventptr->evtime;    /* update time to next event time */
        if (eventptr->evtype == FROM_LAYER2 ) {
            if (eventptr->eventity == 0) 
                rtupdate0(eventptr->rtpktptr);
            else if (eventptr->eventity == 1) 
                rtupdate1(eventptr->rtpktptr);
            else if (eventptr->eventity == 2) 
                rtupdate2(eventptr->rtpktptr);
            else if (eventptr->eventity == 3) 
                rtupdate3(eventptr->rtpktptr);
           /* else if (eventptr->eventity == 4)
                rtupdate4(eventptr->rtpktptr);
            else if (eventptr->eventity == 5) 
                rtupdate5(eventptr->rtpktptr);
            else if (eventptr->eventity == 6) 
                rtupdate6(eventptr->rtpktptr);
            else if (eventptr->eventity == 7) 
                rtupdate7(eventptr->rtpktptr);
            else if (eventptr->eventity == 8) 
                rtupdate8(eventptr->rtpktptr);
            else if (eventptr->eventity == 9) 
                rtupdate9(eventptr->rtpktptr);*/
            else { printf("Panic: unknown event entity\n"); exit(0); }
        }
	/*
        else if (eventptr->evtype == LINK_CHANGE ) {
            if (clocktime<10001.0) {
              linkhandler0(1,20);
              linkhandler1(0,20);
              }
            else   {
              linkhandler0(1,1);
              linkhandler1(0,1);
              }
          }
	  */
          else
             { printf("Panic: unknown event type\n"); exit(0); }
        if (eventptr->evtype == FROM_LAYER2 ) 
          free(eventptr->rtpktptr);        /* free memory for packet, if any */
        free(eventptr);                    /* free memory for event struct   */
      }
   

terminate:
   printf("\nSimulator terminated at t=%f, no packets in medium\n", clocktime);
   return(0);
}



//////////////////////////////////////////////////////////////////////
//  init()
//  This routine initializes lots of parameters and variables used
//  throughout the simulation.
//////////////////////////////////////////////////////////////////////
void  init()  {              
    int i, j, found;
    float sum, avg;
    struct event *evptr;  
    FILE *fp;                 // Used for the configuration file
  

    srand(9999);              // init random number generator 
    sum = 0.0;                // test random number generator for students 
    for (i = 0; i < 1000; i++)
        sum = sum + GetRandomNumber();    //  should be uniform in [0,1] 
    avg = sum/1000.0;
    if (avg < 0.25 || avg > 0.75) {
        printf("It is likely that random number generation on your machine\n" ); 
        printf("is different from what this emulator expects.  Please take\n");
        printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
        exit(0);
    }
    clocktime = 0.0;                // initialize time to 0.0 
    
    // Load in node information from the configuration file
    fp = fopen( ConfigurationFilename, "r" );
    if ( fp == NULL )  {
        printf("Unable to open the configuration file %s\n", ConfigurationFilename );
        exit(0);
    }
    // We only expect ONE value on the first line, but guard against 
    // someone having MORE than one number.
    found = fscanf( fp,  "%d, %d", &NumberOfNodes, &i );
    if ( found != 1 )  {    // We expect 1 number on the first line
        printf( "First line of file should contain number of nodes\n");
        exit(0);
    }
    if (NumberOfNodes < 2 || NumberOfNodes > MAX_NODES )  {
        printf( "Error reading %s configuration file - number of nodes\n", ConfigurationFilename );
        exit(0);
    }

    // Initialize the array so illegal nodes have infinite cost
    for ( i = 0; i < MAX_NODES; i++ )
        for ( j = 0; j < MAX_NODES; j++ )
            ConnectCosts[i][j] = INFINITY;

    // Now load in the nodes and costs from the file
    for ( i = 0; i < NumberOfNodes; i++ )  {
        found = fscanf( fp, "%d, %d, %d, %d", //, %d, %d, %d, %d, %d, %d",
            &ConnectCosts[i][0], &ConnectCosts[i][1], &ConnectCosts[i][2],
            &ConnectCosts[i][3]); /*&ConnectCosts[i][4], &ConnectCosts[i][5],
            &ConnectCosts[i][6], &ConnectCosts[i][7], &ConnectCosts[i][8],
            &ConnectCosts[i][9] */
        if ( found != NumberOfNodes )  {
            printf("We expected to see %d node costs on this line but found %d\n",
                    NumberOfNodes, found );
            exit(0);
        }
    }
    fclose(fp);    // We're all done

    // Call each of the nodes so they can initialize their stuff
    rtinit0();
    rtinit1();
    rtinit2();
    rtinit3();
  /*  rtinit4();
    rtinit5();
    rtinit6();
    rtinit7();
    rtinit8();
    rtinit9(); */

    /* initialize future link changes */
    if (LINKCHANGES == 1)   {
        evptr = (struct event *)malloc(sizeof(struct event));
        evptr->evtime =  10000.0;
        evptr->evtype =  LINK_CHANGE;
        evptr->eventity =  -1;
        evptr->rtpktptr =  NULL;
        insertevent(evptr);
        evptr = (struct event *)malloc(sizeof(struct event));
        evptr->evtype =  LINK_CHANGE;
        evptr->evtime =  20000.0;
        evptr->rtpktptr =  NULL;
        insertevent(evptr);    
    }
}       // End of init()


/**************************************************************************
    GetRandomNumber()
        This is a method given by Jain.  It calculates

              X = ( 16807 * X ) % 2147483647.

        However this requires 48-bit arithmetic so the beauty of
        the method is that he reduces it to run on 32 bits (and
        thus is much faster).

    Returns a double between 0 and 1;
    August 2004:  Made thread safe.  Having a static here produced strange
                  results.  When multithreaded, we may return two of the
                  same random number to different threads.
**************************************************************************/

long static RandomSeed = 42;

void     SetRandomSeed( long NewRandomSeed )
{
    RandomSeed = NewRandomSeed;
}
double   GetRandomNumber( )
    {
    long                a = 16807;            /* Multiplier */
    long                m = 2147483647;       /* Modulus    */
    long                q = 127773;           /* m / a      */
    long                r = 2836;             /* m mod a    */
    long                X_div_q;
    long                X_mod_q;
    long                Working;

    Working = RandomSeed;
    if ( Working == 0 )
        Working = 1;
    X_div_q = Working / q;
    X_mod_q = Working % q;
    Working       = a * X_mod_q - r * X_div_q;
    if ( Working < 0 )
        Working = Working + m;
    RandomSeed = Working;
    return( ((double)Working)/(double)m  );
}                                /* End of GetRandomNumber             */

/********************* EVENT HANDLINE ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/
 
void  insertevent( struct event *p ) {
    struct event *q,*qold;

    if (TraceLevel > 3) {
        printf("            INSERTEVENT: time is %lf\n",clocktime);
        printf("            INSERTEVENT: future time will be %lf\n",p->evtime); 
    }
    q = evlist;     /* q points to header of list in which p struct inserted */
    if (q == NULL) {   /* list is empty */
        evlist = p;
        p->next = NULL;
        p->prev = NULL;
        }
    else {
        for (qold = q; q !=NULL && p->evtime > q->evtime; q=q->next)
             qold = q; 
        if (q == NULL) {   /* end of list */
             qold->next = p;
             p->prev = qold;
             p->next = NULL;
        }
        else if (q == evlist) { /* front of list */
             p->next = evlist;
             p->prev = NULL;
             p->next->prev = p;
             evlist = p;
        }
        else {     /* middle of list */
             p->next = q;
             p->prev = q->prev;
             q->prev->next = p;
             q->prev = p;
         }
    }    // End of list is not null
}

// Print everything that is in the eventlist
void  printevlist()    {
    struct event *q;
    printf("--------------\nEvent List Follows:\n");
    for(q = evlist; q!=NULL; q=q->next) {
        printf("Event time: %f, type: %d entity: %d\n",q->evtime,q->evtype,q->eventity);
    }
    printf("--------------\n");
}

//////////////////////////////////////////////////////////////////////
//    getNeighborCosts(int myNodeNumber)
//    This is used by the nodes to discover their local neighbors
//    and the total number of nodes in the network.
//    Input:  The node number of that node
//    Output: A structure containing the local information
//  
//    Note that we never free this structure so the user in the node
//    can continue to use this pointer without fear of the data 
//    disappearing.
//////////////////////////////////////////////////////////////////////
struct NeighborCosts *getNeighborCosts(int myNodeNumber)  {
    int   i;
    struct NeighborCosts *returnStruct 
        = (struct NeighborCosts *)malloc( sizeof (struct NeighborCosts));

    for ( i = 0; i < MAX_NODES; i++ )  {    // Initialize the array
        returnStruct->NodeCosts[i] = INFINITY;
    }

    // Now fill in the structure.
    returnStruct->NodesInNetwork = NumberOfNodes;
    for ( i = 0; i < MAX_NODES; i++ )  {
        returnStruct->NodeCosts[i] = ConnectCosts[myNodeNumber][i];
    }
    return( returnStruct  );
}     // End of getNeighborCosts

//////////////////////////////////////////////////////////////////////
//  toLayer2()
//  This is called by the student's node in order to send a packet
//  to some other node.  This is the communication medium that's
//  used.  There is little knowledge of the actual network here 
//  other than to check that the student is sending data to a
//  neighbor rather than to some strange place.
//////////////////////////////////////////////////////////////////////
/************************** TOLAYER2 ***************/
void   toLayer2( struct RoutePacket packet ) {
    struct RoutePacket *mypktptr;
    struct event *evptr, *q;
    float lastime;

    int i;
    
    // be nice: check if source and destination id's are reasonable 
    if (packet.sourceid < 0 || packet.sourceid > NumberOfNodes ) {
         printf("WARNING: illegal source id in your packet, ignoring packet!\n");
         return;
    }
    if (packet.destid < 0 || packet.destid > NumberOfNodes ) {
        printf("WARNING: illegal dest id in your packet, ignoring packet!\n");
        return;
    }
    if (packet.sourceid == packet.destid)  {
        printf("WARNING: source and destination id's the same, ignoring packet!\n");
        return;
    }
    if (ConnectCosts[packet.sourceid][packet.destid] == INFINITY)  {
        printf("WARNING: source and destination not connected, ignoring packet!\n");
        return;
    }

// make a copy of the packet student just gave me since he/she may decide 
// to do something with the packet after we return back to him/her *
    mypktptr = (struct RoutePacket *) malloc(sizeof(struct RoutePacket));
    mypktptr->sourceid = packet.sourceid;
    mypktptr->destid = packet.destid;
    for (i = 0; i < NumberOfNodes; i++)
       mypktptr->mincost[i] = packet.mincost[i];
    if (TraceLevel>2)  {
         printf("    TOLAYER2: source: %d, dest: %d\n              costs:", 
             mypktptr->sourceid, mypktptr->destid);
         for (i=0; i < NumberOfNodes; i++)
              printf("%d  ",mypktptr->mincost[i]);
         printf("\n");
    }

// create future event for arrival of packet at the other side 
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtype =  FROM_LAYER2;     // packet will pop out in layer3 
    evptr->eventity = packet.destid;  // event occurs at other entity 
    evptr->rtpktptr = mypktptr;       // save ptr to my copy of packet 

// finally, compute the arrival time of packet at the other end.
// medium can not reorder, so make sure packet arrives between 1 and 10
// time units after the latest arrival time of packets
// currently in the medium on their way to the destination 
    lastime = clocktime;
    for (q=evlist; q!=NULL ; q = q->next) 
       if ( (q->evtype == FROM_LAYER2  && q->eventity == evptr->eventity) ) 
            lastime = q->evtime;
    evptr->evtime =  lastime + 2.*GetRandomNumber();

 
    if (TraceLevel>2)  
        printf("    TOLAYER2: scheduling arrival on other side\n");
    insertevent(evptr);
}     // End of toLayer2

