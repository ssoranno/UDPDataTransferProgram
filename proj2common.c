#ifndef PROJ2COMMON_H_
#define PROJ2COMMON_H_
#include <stdio.h>      /* for standard I/O functions */
#include <stdlib.h>
#include "proj2common.h"

// Return 0 if not lost, 1 if lost
// Threshold is between 0 and 1
int simulateLoss(float lossRate)
{
    double r = (double)rand()/(double)RAND_MAX;
    if(r < lossRate)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// Return 0 if not lost, 1 if lost
// Threshold is between 0 and 1
int simulateACKLoss(float lossRate)
{
    return simulateLoss(lossRate);
}

// Prints some relevant information about the packet
void printPacketInfo(struct Packet* pktptr)
{
    printf("== Packet Info:\n");
    printf("Count: %d, SeqNum: %d\n", htons((*pktptr).count), htons((*pktptr).seqNum));
    printf("== Data:\n%s\n", (*pktptr).data);
}

#endif