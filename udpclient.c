/* udp_client.c */
/* Programmed by Steven Soranno and Viroon Yong */
/* May 8, 2018 */

#include <stdio.h>      /* for standard I/O functions */
#include <stdlib.h>     /* for exit */
#include <string.h>     /* for memset, memcpy, and strlen */
#include <netdb.h>      /* for struct hostent and gethostbyname */
#include <sys/socket.h> /* for socket, sendto, and recvfrom */
#include <netinet/in.h> /* for sockaddr_in */
#include <unistd.h>     /* for close */
#include "proj2common.h"/* Contains structs and functions to create data and ack packets */

// Hardcoded server hostname and server port
// Should be changed if using a different server or port number
#define server_hostname "cisc450.cis.udel.edu"
#define server_port 46970

// Main
/*
Command-Line Args:
./udpclient <lossRate> <lossRate_ACK>
*/
int main(int argc, char *argv[])
{
    // Default loss rates are set to 0.0
    float lossRate = 0.0;
    float lossRate_ACK = 0.0;
    // Assign lossRate command-line arguments
    if(argc == 3)
    {
        lossRate = atof(argv[1]); 
        lossRate_ACK = atof(argv[2]);
    }
    else
    {
        printf("Invalid parameters\n");
        return 2;
    }
    
    // Check that the loss rates are valid
    if(lossRate > 1 || lossRate < 0 || lossRate_ACK > 1 || lossRate_ACK < 0)
    {
        printf("Invalid input for loss rates\n");
        return 2;
    }

    printf("Packet Loss Rate: %f, ACK Loss Rate: %f\n", lossRate, lossRate_ACK);

    int sock_client; /* Socket used by client */

    struct sockaddr_in client_addr; /* Internet address structure that
                                        stores client address */
    unsigned short client_port;     /* Port number used by client (local port) */

    struct sockaddr_in server_addr; /* Internet address structure that
                                        stores server address */
    struct hostent *server_hp;      /* Structure to store server's IP
                                        address */

    char data[PACKET_MAX_LENGTH]; // String used for storing the message to be sent

    unsigned int msg_len;       /* length of message */
    int bytes_sent, bytes_recd; /* number of bytes sent or received */

    /* open a socket */

    if ((sock_client = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        perror("Client: can't open datagram socket\n");
        exit(1);
    }

    /* initialize client address information */

    client_port = 0; /* This allows choice of any available local port */

    /* Uncomment the lines below if you want to specify a particular 
             local port: */

    /* clear client address structure and initialize with client address */
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* This allows choice of
                                        any host interface, if more than one 
                                        are present */
    client_addr.sin_port = htons(client_port);

    /* bind the socket to the local client port */

    if (bind(sock_client, (struct sockaddr *)&client_addr,
             sizeof(client_addr)) < 0)
    {
        perror("Client: can't bind to local address\n");
        close(sock_client);
        exit(1);
    }

    /* end of local address initialization and binding */

    /* initialize server address information */

    if ((server_hp = gethostbyname(server_hostname)) == NULL)
    {
        perror("Client: invalid server hostname\n");
        close(sock_client);
        exit(1);
    }

    /* Clear server address structure and initialize with server address */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy((char *)&server_addr.sin_addr, server_hp->h_addr,
           server_hp->h_length);
    server_addr.sin_port = htons(server_port);

    /* user interface */

    // Get the filename
    printf("Please input name of file:\n");
    scanf("%s", data);
    msg_len = strlen(data) + 1;

    /* send message */
    struct Packet fname;     // Struct for packet
    fname.seqNum = ntohs(0); // set the sequence number and size of the data in the packet
    fname.count = ntohs(msg_len);
	
	// Populate the string data array in the packet struct
    int i;
    for (i = 0; i < PACKET_MAX_LENGTH; i++)
    {
        fname.data[i] = data[i];
    }
	// Send the filename packet to the server
    bytes_sent = sendto(sock_client, &fname, strlen(fname.data)+4, 0,
                        (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Clear the output file
    FILE *file = fopen("out.txt", "w");
    fclose(file);
    // Open file for appending
    file = fopen("out.txt", "a");

    int totalBytesRec = 0; // Number of bytes received
    int numPacketsRec = 0; // Number of packets received
    int eofReached = 0; // boolean end of reached
    int expSeqNum = 0; // expected sequence number
    int lossPack = 0; // number of packets lost
    int numAck = 0; // number of acks
    int totalPacket =0; // number of total packets recieved
    int numAckLoss= 0; // number of acks lost
    int totalAck = 0; // total acks send
    int dupPack = 0; // number of duplicate packets
	
	// Clear the data array with null characters
    memset(data, '\0', PACKET_MAX_LENGTH);

    // Loop until all end of transmission packet is received
    while (!eofReached)
    {
        struct Packet p;
		
		// Recieve data packet from the server
        bytes_recd = recvfrom(sock_client, &p, sizeof(p), 0, (struct sockaddr *)0, (int *)0);
		totalPacket++;
		
        if(simulateLoss(lossRate) == 1) // simulate Packet loss
        {
            printf("Packet %d lost\n", expSeqNum);
			lossPack++;
        }
        else
        {
            // Extract the data
            p.count = htons(p.count);
            p.seqNum = htons(p.seqNum);
            if (p.count != 0)
            {
                printf("Packet %d received with %d data bytes\n", p.seqNum, p.count);
				// Increment statistic variables
				numPacketsRec += 1;
				totalBytesRec += p.count;
			}
			
			// if the packet contains the correct sequence numbers and is not the final packet
            if (p.seqNum == expSeqNum && p.count != 0)
            {
				// create ack packet
                struct ACKPacket a;
                a.ACKNum = ntohs(expSeqNum);
                expSeqNum = 1 - expSeqNum;
				// Add a null character to the end of the data string for writing to the file
				p.data[p.count] = '\0';
                int temp = fputs(p.data, file);// write to "out.txt"
				
				// simulate ACK loss or sent the ack to the server
                if(simulateACKLoss(lossRate_ACK) == 1)
                {
                    printf("ACK %d lost\n", htons(a.ACKNum));
					numAckLoss++;
                }
                else
                {
                    bytes_sent = sendto(sock_client, &a, sizeof(a), 0,
                        (struct sockaddr *)&server_addr, sizeof(server_addr));
                    printf("ACK %d transmitted\n", htons(a.ACKNum));
					numAck++;
                }
				totalAck++;
            }
            else if (p.count != 0) // if the packet recieved contains incorrect sequence numbers
            {
                // Send Ack with 1- the expected sequence number
				struct ACKPacket a;
                a.ACKNum = ntohs(1 - expSeqNum);
                printf("Duplicate Packet %d received with %d data bytes\n", p.seqNum, p.count);
                
				// Simulate Ack loss or send the ack
				if(simulateACKLoss(lossRate_ACK) == 1)
                {
                    printf("ACK %d lost\n", htons(a.ACKNum));
					numAckLoss++;
                }
                else
                {
                    bytes_sent = sendto(sock_client, &a, sizeof(a), 0,
                                        (struct sockaddr *)&server_addr, sizeof(server_addr));
                    printf("ACK %d transmitted\n", htons(a.ACKNum));
					dupPack++;
                }
				totalAck++;
            }
            // Check if the EOF packet was sent and break the loop
            if (p.count == 0)
            {
                printf("End of Transmission Packet with sequence number %d received with %d data bytes\n", p.seqNum, bytes_recd);
                eofReached = 1;
            }
        }
        // Write the data from the packet to the output file
    } // while (!eofReached)

	// print statistics
    printf("Statistics:\n");
    printf("Number of data packets received successfully (without loss, without duplicates):%d\n", numPacketsRec);
    printf("Total number of data bytes received:%d\n", totalBytesRec);
    printf("Total number of duplicate data packets received:%d\n", dupPack);
    printf("Number of data packets received but dropped due to loss:%d\n", lossPack);
    printf("Total number of data packets received:%d\n", totalPacket);
    printf("Number of ACKs transmitted without loss:%d\n",numAck);
    printf("Number of ACKs generated but dropped due to loss:%d\n",numAckLoss);
    printf("Total number of ACKs generated (with and without loss):%d\n", totalAck);
    // close the file
    fclose(file);

    /* close the socket */
    printf("Client socket closed with code %d\n", close(sock_client));

    /* close the socket */

    close(sock_client);
}
