/**************************************************************************/
//  Richard Bromley 5001075854
//  CS 370 1001 Assignment 4
//
//  Peterson Leader Election: Using threads with shared memory locations
//
//
//
//  Location of file: Assignment4
/**************************************************************************/
#define MAX_NODES 50
#define TRUE 1
#define FALSE 0
#define ACTIVE 2
#define RELAY 1

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

struct Radio {
    int uid;
    int buffer;

    sem_t TRANSMITTER;
    sem_t SYN;

    struct Radio *reciever;    //we recieve from here
};
//global variables
int loaded = FALSE;
int allLoaded = FALSE;
int leader = FALSE; 
int nodeCount = 0;

struct Radio *channel[MAX_NODES]; //Addresses upto MAX_NODES radios
/**************************************************************************/
//  Richard Bromley 5001075854
//  channel transmitter  
// I had to add a compound lock preventing writes before or during reads
// The odds of a race condition compound with high levels of threads.
// I included a an empty buffer test that allows greater numbers of threads.
// It works without the test---errors can happen though--even with a sync
// semaphore.
/**************************************************************************/
void write_channel (struct Radio *radio, int word) {
  while(radio->buffer != 0) { }
    sem_wait(&radio->TRANSMITTER); 	//lock transmission buffer
    radio->buffer = word;         	//write word
    sem_post(&radio->SYN);         	//allow read of buffer
    sem_post(&radio->TRANSMITTER); 	//unlock transmission buffer
    }
/**************************************************************************/
//  Richard Bromley 5001075854
//  channel reciever
/**************************************************************************/
int read_channel (struct Radio *radio) {
    int word;// if nothing transmitted nothing to read
    sem_wait(&radio->reciever->SYN);
    sem_wait(&radio->reciever->TRANSMITTER); 	//lock reciver buffer
    word = radio->reciever->buffer;
    radio->reciever->buffer = 0;                //write word
    sem_post(&radio->reciever->TRANSMITTER); 	//unlock reciever buffer
    return word;
    }
/**************************************************************************/
//  Richard Bromley 5001075854
//
//  Two channels will share one memory location
//  The sibling transmitter is the Channel reciever
//
//  Each channel has only one buffer, which is the write buffer.  Reads 
//  are grabbed from threads liken-to a linked list.
//  channel reciever
/**************************************************************************/
void *Channel(void *xptr) {
    int *ptr = (int *)xptr;
    int address = (*ptr);

    int phase = 1;    
    int status = ACTIVE; //0 = active 1 = relay

    struct Radio *radio;
    radio = channel[address];

    int tempid = radio->uid;
    int oneHop,twoHop;

    write_channel (radio, tempid); //The first write is rushed for active
                                   // active threads.

    loaded = TRUE;  //notify main() the thread has accepted its
                    //data structure.

    while(!allLoaded) {}  // all active threads await the total
                          // creation of all threads.  This isn't
                          // really needed though many threads
                          // can race ahead in phases while some
                          //await activation from by the OS..

    //Reciever Address
    if(address > 0) radio->reciever = channel[address - 1];
    else radio->reciever = channel[nodeCount - 1];

    oneHop = read_channel (radio);
    write_channel (radio, oneHop);
    twoHop = read_channel (radio);
    
    while(printf("[%d][%d][%d] \n",phase,radio->uid,tempid) < 3) { }
    ++phase;
    // The logic meets the outline literally.
    while(leader == 0) {
	    if(status == RELAY) {
		oneHop = read_channel (radio);
            	write_channel (radio, oneHop);
            	oneHop = read_channel (radio);
		write_channel (radio, oneHop);
		++phase;
		continue;            
	    }
            if(oneHop == tempid) leader = radio->uid;
            else if(oneHop > twoHop && oneHop > tempid) {
	        tempid = oneHop;
		while(printf("[%d][%d][%d] \n",phase,radio->uid,tempid) < 3) {  }
	    }    
	    else {
		status = RELAY;
		++phase;
		continue;
	    }
	     write_channel (radio, tempid); 
	     oneHop = read_channel (radio);
    	     write_channel (radio, oneHop);
    	     twoHop = read_channel (radio);
	     ++phase;
    }
    return NULL;
}
