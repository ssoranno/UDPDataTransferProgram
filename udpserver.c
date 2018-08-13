/* udp_server.c */
/* Programmed by Steven Soranno and Viroon Yong */
/* February 21, 2018 */

#include <ctype.h>      /* for toupper */
#include <stdio.h>      /* for standard I/O functions */
#include <stdlib.h>     /* for exit */
#include <string.h>     /* for memset */
#include <sys/socket.h> /* for socket, sendto, and recvfrom */
#include <netinet/in.h> /* for sockaddr_in */
#include <unistd.h>     /* for close */
#include <math.h>
#include <time.h>
#include "proj2common.h"

#define STRING_SIZE 1024

/* SERV_UDP_PORT is the port number on which the server listens for
   incoming messages from clients. You should change this to a different
   number to prevent conflicts with others in the class. */

// Hardcoded server port
// Should be changed if using a difference port number
#define SERV_UDP_PORT 46970

// makePacket function populates the pkt struct with the data, count(size of the string array),
// and the sequence number of the packet
void makePacket(char data[], struct Packet* pktptr)
{
    (*pktptr).count = ntohs(strlen(data));     //The length of the line is the packet length.
    (*pktptr).seqNum = ntohs(1 - htons((*pktptr).seqNum));

    // Assign the data
    int i;
    for (i = 0; i < PACKET_MAX_LENGTH; i++)
    {
        (*pktptr).data[i] = data[i];
    }
}

int main(int argc, char *argv[])
{
    // Timeout in 10^n microseconds
    int timeout_n = 0; // If a user enters a decimal for this, it always rounds down (floor)
    unsigned long int timeout_museconds = 0;
    struct timeval timeout;
    // Assign timeout_n command-line argument
    if(argc == 2)
    {
        timeout_n = atof(argv[1]);
        if(timeout_n > 10 || timeout_n < 0) // Check bounds
        {
            printf("Invalid input for timeout n\n");
            return 2;
        }
        else
        {
            if(timeout_n >= 6) // Over 1000000 microseconds -> convert to seconds
            {
                timeout.tv_sec = pow(10, timeout_n - 6);
                timeout.tv_usec = 0;
            }
            else 
            {
                timeout.tv_sec = 0;
                timeout.tv_usec = pow(10, timeout_n);
            }
        }
    }
    else
    {
        printf("Invalid parameters\n");
        return 2;
    }

	printf("Timeout: %ld.%06ld seconds\n", timeout.tv_sec, timeout.tv_usec);

    int sock_server; /* Socket on which server listens to clients */

    struct sockaddr_in server_addr; /* Internet address structure that
                                        stores server address */
    unsigned short server_port;     /* Port number used by server (local port) */

    struct sockaddr_in client_addr; /* Internet address structure that
                                        stores client address */
    unsigned int client_addr_len;   /* Length of client address structure */
    int bytes_sent, bytes_recd;         /* number of bytes sent or received */
    struct Packet pkt;
    struct ACKPacket ackpkt;
	

    /* open a socket */

    if ((sock_server = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        perror("Server: can't open datagram socket\n");
        exit(1);
    }

    /* initialize server address information */

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* This allows choice of
                                        any host interface, if more than one
                                        are present */
    server_port = SERV_UDP_PORT;                     /* Server will listen on this port */
    server_addr.sin_port = htons(server_port);

    /* bind the socket to the local server port */

    if (bind(sock_server, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("Server: can't bind to local address\n");
        close(sock_server);
        exit(1);
    }


    /* wait for incoming messages in an indefinite loop */
    printf("Waiting for incoming messages on port %hu\n\n", server_port);
    
    client_addr_len = sizeof(client_addr);
    // Receive the filename packet
    bytes_recd = recvfrom(sock_server, &pkt, STRING_SIZE, 0,
                          (struct sockaddr *)&client_addr, &client_addr_len);
    pkt.count = htons(pkt.count);
    pkt.seqNum = htons(pkt.seqNum);
	pkt.data[pkt.count - 1] = '\0'; // add null character to the end of the file name
    printf("The file name is %s\n", pkt.data);
	
    // Decide which file to open based on the filename received.
    // open the file
    FILE *file = fopen(pkt.data, "r");
    int lineNum = 1;    // Sequence number = file line number

    // Statistics
    int totalBytes = 0; // Total number bytes
    int pktsTransmitted = 0; // number of packets transmitted initially
    int pktsRetransmitted = 0; // number of packets retransmitted
    int totalPktsTransmitted = 0; // Total packets transmitted
    int numAcksRcvd = 0; // number of acks received 
    int numTimeouts = 0; // number of timeouts

    // If the file exists, start reading the file and creating packets to be sent
    if (file)
    {
        pkt.seqNum = ntohs(1);   // Reset pkt seqnum
        ackpkt.ACKNum = 1;
        char data[PACKET_MAX_LENGTH];
        // loop through every line in the file
        while (fgets(data, PACKET_MAX_LENGTH, file))
        {
            bytes_recd = 0; // Reset bytes_recd

            makePacket(data, &pkt); // Create the packet for transmission
			
			// Loop until there is no timeout
			int noTimeout = 1;
			while(noTimeout){
				// increament the statistics variables
                totalPktsTransmitted++;
				// Send the packet
				bytes_sent = sendto(sock_server, &pkt, strlen(data)+4, 0,
									(struct sockaddr *)&client_addr, client_addr_len);
				if (bytes_sent == -1)
				{
					printf("Error in sending the packet\n");
				}
				else
				{
					printf("Packet %d transmitted with %d data bytes\n", ntohs(pkt.seqNum), ntohs(pkt.count));
				}
				
				// set timeout for socket
                setsockopt(sock_server, SOL_SOCKET, SO_RCVTIMEO, (const void *) &timeout, sizeof(timeout));
                // Wait for the ACK
				bytes_recd = recvfrom(sock_server, &ackpkt, sizeof(ackpkt), 0,
							(struct sockaddr *)&client_addr, &client_addr_len);
				if(bytes_recd >= 0) // ACK received
				{
					printf("ACK %d Received\n", ntohs(ackpkt.ACKNum));
                    pktsTransmitted++;
					numAcksRcvd++;
                    noTimeout = 0;
				}
				else // ACK not received
				{
                    numTimeouts++;
					noTimeout = 1;
				}
			}
			// Increment variables
			totalBytes += strlen(data) + 1;
			lineNum++;
			
        }

        // Populate and Send the final packet
        pkt.count = ntohs(0);
        pkt.seqNum = ntohs(1-pkt.seqNum);
        bytes_sent = sendto(sock_server, &pkt, pkt.count+4, 0,
                            (struct sockaddr *)&client_addr, client_addr_len);
        printf("End of Transmission Packet with sequence number %d transmitted with %d data bytes\n", htons(pkt.seqNum), pkt.count);

    }
    else
    {
        printf("File not found\n");
        return 3;
    }
	
	// Calculate the packets retransmitted
    pktsRetransmitted = totalPktsTransmitted - pktsTransmitted;

    // Print statistics
    printf("Number of data packets transmitted (initial transmission only): %d\n", pktsTransmitted);
    printf("Total number of data bytes transmitted: %d\n", totalBytes);
    printf("Total number of retransmissions: %d\n", pktsRetransmitted);
    printf("Total number of data packets transmitted (initial transmissions plus retransmissions): %d\n", totalPktsTransmitted);
    printf("Number of ACKs Received: %d\n", numAcksRcvd);
    printf("Number of Timeouts: %d\n", numTimeouts);
	
	// close the file
    fclose(file);
}
