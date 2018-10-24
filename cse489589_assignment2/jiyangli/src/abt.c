#include "../include/simulator.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <getopt.h>


/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

/********* STUDENTS WRITE THE NEXT SIX ROUTINES *********/
int msgc;
int seq;
int ack;
float increment;
struct pkt *msg_pkt;

int checksum(char *str){
    int sum = 0;
    while (int j=0, j<=20) {
        sum += str[j];
        j++;
    }
    return sum;
}

void make_pkt(struct msg message, int seq, int checksum){
    msg_pkt->seqnum = seq;
    msg_pkt->acknum = seq;
    if (message==NULL) {
        msg_pkt->checksum = msg_pkt->seqnum + msg_pkt->acknum;
    }
    else{
        msg_pkt->checksum = checksum + msg_pkt->seqnum + msg_pkt->acknum;
        strcpy(msg_pkt->payload, message.data);
    }
}

/* called from layer 5, passed the data to be sent to other side */
void A_output(message)
  struct msg message;
{
    // call 0 from above
    if(seq==0){
        checks = checksum(message.data);
        msg_pkt = make_pkt(seq, buffer[msgc], checks);
        tolayer3(0, msg_pkt);
        msgc++;
        starttimer(0, increment);
        seq = 1;
    }
    // call 1 from above
    if(seq==1){
        checks = checksum(message.data);
        msg_pkt = make_pkt(seq, buffer[msgc], checks);
        tolayer3(0, msg_pkt);
        msgc++;
        starttimer(0, increment);
        seq = 0;
    }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{   check_ack = packet.seqnum + packet.acknum;
    if(check_ack==packet.checksum){
        
        if(packet.acknum==0){
            tolayer5(0, message);
            stoptimer(0);
        }
        if(packet.acknum==1){
            tolayer5(0, message);
            stoptimer(0);
        }
    }
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
    tolayer3(0, msg_pkt);
    starttimer(0, increment);

}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
    int msgc = 0;
    int seq = 0;
    int ack = 0;
    float increment = 15;

}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
  struct pkt packet;
{
    check_msg = packet.seqnum + packet.acknum + checksum(packet.payload);
    if(check_msg==packet.checksum){
        if(packet.seqnum==0){
            make_pkt(packet.seqnum, NULL, check_msg);
            tolayer3(1, )
        }
    }

}

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{

}
