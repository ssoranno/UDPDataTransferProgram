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
The client begins by promting the user for the name of the file they wish to receive from the server.  Once the user enters the filename, a packet is created containing the filename and is sent to the server.  The client then enters the the rdt3.1 FSM for a receiver.<br>

The client has only one state, "Wait for call from below".  When the client recieves a data packet from the server, it checks to make sure the sequence number on that packet is correct and matches the one the client expects.  If the sequence number is not correct, the client sends an ACK packet back to the server with 1 subtracted from the expected sequence number.  If the sequence is correct, the client sends an ACK packet to the sever with the expected sequence number.  Then the sequence number is changed and the client waits for the next data packet.

### Output
When the client and server programs run, they print statisticcs based on the which packets were sent an received.  The symbol “n” below refers to the sequence number of the transmitted or received packet (note that the sequence number must always be either 0 or 1), and the symbol “c” below refers to the count (number of data bytes) in the transmitted or received packet.<br>

The messages to be printed by the server are:<br>

When a new data packet numbered n is sent by the server:<br>
⋅⋅⋅*Packet n transmitted with c data bytes*<br>
When a data packet numbered n is retransmitted by the server:<br>
⋅⋅⋅*Packet n retransmitted with c data bytes*<br>
When a timeout expires:<br>
⋅⋅⋅*Timeout expired for packet numbered n*<br>
When an ACK is received with number n:<br>
⋅⋅⋅*ACK n received*<br>
When the “End of Transmission” packet is sent:<br>
⋅⋅⋅*End of Transmission Packet with sequence number n transmitted with c data bytes*<br>

The messages to be printed by the client are:<br>

When a data packet numbered n is received correctly by the client for the first time:<br>
⋅⋅⋅*Packet n received with c data bytes*<br>
When a data packet numbered n is received correctly by the client, but is a duplicate packet:<br>
⋅⋅⋅*Duplicate packet n received with c data bytes*<br>
When a data packet numbered n is received by the client, but the SimulateLoss function causes it to be dropped:<br>
⋅⋅⋅*Packet n lost*<br>
When an ACK is generated with number n, but is dropped by the function SimulateACKLoss:<br>
⋅⋅⋅*ACK n lost*<br>
When an ACK is generated with number n and transmitted without loss:<br>
⋅⋅⋅*ACK n transmitted*<br>
When the “End of Transmission” packet is received:<br>
⋅⋅⋅*End of Transmission Packet with sequence number n received with c data bytes*<br>

At the end, before terminating execution, the following statistics will be printed. 

For server:

Number of data packets transmitted (initial transmission only)<br>
Total number of data bytes transmitted (this should be the sum of the count fields of all transmitted packets when transmitted for the first time only)<br>
Total number of retransmissions<br>
Total number of data packets transmitted (initial transmissions plus retransmissions)<br>
Number of ACKs received<br>
Count of how many times timeout expired<br>

For client:

Number of data packets received successfully (without loss, without duplicates)<br>
Total number of data bytes received which are delivered to user (this should be the sum of the count fields of all packets received successfully without loss without duplicates)<br>
Total number of duplicate data packets received (without loss)<br>
Number of data packets received but dropped due to loss<br>
Total number of data packets received (including those that were successful, those lost, and duplicates)<br>
Number of ACKs transmitted without loss<br>
Number of ACKs generated but dropped due to loss<br>
Total number of ACKs generated (with and without loss)<br>
