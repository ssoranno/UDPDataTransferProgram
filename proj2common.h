#define PACKET_MAX_LENGTH 80

struct Packet // UDP Segment
{
    // Header fields
    unsigned short seqNum;
    unsigned short count;
    // Payload
    char data[PACKET_MAX_LENGTH];
};

struct ACKPacket
{
    unsigned short ACKNum;
};

int simulateLoss(float lossRate);
int simulateACKLoss(float lossRate);
void printPacketInfo(struct Packet* pktptr);