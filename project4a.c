/**************************************************************************/
//  Richard Bromley 5001075854
//  CS 370 1001 Assignment 4//
//  Peterson Leader Election: Using threads with shared memory locations
//
//
//
//  Location of file: Assignment4
/**************************************************************************/
#include "project4a.h"

int main(int argc, char *argv[]) {
    struct Radio *firstNode;
    int address = 0;
    char semaphoreString[6];
    
 	//count of nodes
    scanf("%d",&nodeCount);    
    pthread_t Channelthread[nodeCount];
    
    	// create all the radios needed
    firstNode = (struct Radio *) malloc(nodeCount * sizeof (struct Radio));
    
    while(address < nodeCount) {
        scanf("%d",&(firstNode + address)->uid);
	(firstNode + address)->buffer = 0;
        
	channel[address] = (firstNode + address);
        loaded = FALSE;
	//semaphore creation only occurs here in main.  One is copied
        // and renamed for clarity.
        while (sem_init(&(firstNode + address)->SYN,2,0) < 0) { }
	while (sem_init(&(firstNode + address)->TRANSMITTER,2,1) < 0) { }
	
	pthread_create(&Channelthread[address],NULL,Channel,&address);

	while (loaded == FALSE) { } //wait for channel construction
	                            //So address can be changed without
                                    //damage to a thread comming active.
        ++address;
    }
    allLoaded = TRUE;              //The list if threads are created.

    while(leader == 0){ }         //Await the leader, then report.
    printf("leader: %d \n",leader);
    return 0;
}
