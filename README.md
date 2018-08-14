# UDP Data Transfer Project

These C programs implement the Stop and Wait Protocol to facilitate the reliable data transfer of a file (test1.txt or test2.txt) over UDP.  Packet loss can be simulated by the client for testing purposes.

## Requirements
C compiler<br>
Bash Shell for Windows or Linux/Mac OS

## How to run?
1. Clone git repo to the client and the server machines.
2. Compile udpclient.c on the client and udpserver.c on the server.
3. Run ./udpclient on the client and ./udpserver on the server.

### Arguments for simulated packet loss
The first argument entered into the udpclient program is the packet loss rate.  The packet loss rate is a number between 0 and 1 that represents the rate that a packet is lost.  A 0.3 packet loss rate means that only 30% of the packets will not be lost.

The second argument entered into the udpserver program is the ACK Loss rate.  The ack loss rate is a number between 0 and 1 that represents the rate that an ack will be lost.  A 0.3 ACK loss rate means that only 30% of the ACKs will not be lost.

Example: ./udpclient test1.txt 0.5 0.5<br>
The example above runs the UDP client with a packet loss rate of 0.5 and an ACK loss of 0.5.

The only arguement for the udpserver program is the timeout.  The timeout is a integer value n in the range 1-10 that represents the timeout range stored as 10^n microseconds.

Example: ./udpserver 2<br>
The example above runs the UDP server with a timeout value of 100 microseconds.

## Design Approach
These programs implements finite state machine for Reliable Data Transfer 3.1 using the Alternating Bit Protocol (ABP).

### UDP Server Actions:
The server begins by waiting for a connection from a client containing the filename to transfer. Once it receives a connection and a filename the server enters the rdt3.1 FSM for a sender.<br>

The server has two states, “Wait for call from above” and “Wait for ACK”.  The server starts in state "Wait for Call from Above" and then when the server receives a connection from a client the server creates a packet with the first line of data in the file and a sequence number of 1.  Then the program moves to the next state which is "Wait for Ack", and it sends the first packet to the server.  When the server receives an ACK from the client, it checks to make sure the sequence number is correct.  If the sequence number it incorrect, the server remains in the "Wait for ACK" state until the timeout is up.  When the timeout is complete and no ACK was received, the server resends the packet.  If the sequence number on the ACK is correct, the server returns to the "Wait for call from above" state which restarts the process.

### UDP Client Actions:
The client begins by promting the user for the name of the file they wish to receive from the server.  Once the user enters the filename, a packet is created containing the filename and is sent to the server.  The client then enters the the rdt3.1 FSM for a receiver.<br>\

The client has only one state, "Wait for call from below".  When the client recieves a data packet from the server, it check to make sure the sequence number on that packet is correct and matches the one the client expects.  If the sequence number 
