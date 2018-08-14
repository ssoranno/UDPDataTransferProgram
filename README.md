# UDP Data Transfer Project

These C programs implement the Stop and Wait Protocol to facilitate the reliable data transfer of a file (test1.txt or test2.txt) over UDP.  Packet loss can be simulated by the client for testing purposes.

## Requirements
C compiler<br>
Bash Shell for Windows or Linux/Mac OS

## How to run?
1. Clone git repo to the client and the server machines.
2. Compile udpclient.c on the client and udpserver.c on the server.
3. Run ./udpclient on the client and ./udpserver on the server.

### Arguements for simulated packet loss
The first arguement entered into the udpclient program is the packet loss rate.  The packet loss rate is a number between 0 and 1 that represents the rate that a packet is lost.  A 0.3 packet loss rate means that only 30% of the packets will not be lost.

The second argument entered into the udpserver program is the ACK Loss rate.  The ack loss rate is a number between 0 and 1 that represents the rate that an ack will be lost.  A 0.3 ACK loss rate means that only 30% of the ACKs will not be lost.

Example: ./udpclient 0.5 0.5<br>
The example above runs the UDP client with a packet loss rate of 0.5 and an ACK loss of 0.5.

## Design Approach
These programs implements finite state machine for rdt3.1 for the Alternating Bit Protocol (ABP). 

### UDP Server Actions:
The server has two states, “Wait for call from above” and “Wait for ACK”

### UDP Client Actions:
