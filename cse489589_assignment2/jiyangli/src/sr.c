#include "../include/simulator.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <getopt.h>
#include "gbn.h"

#define MSG_SIZE 1000  /* maximum number of messages can buffer */
#define N 10           /* window size */

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

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
static int msgc;
static int seq;
static int exp_seq;
static int next_seq;
static int base;
static float increment;
static struct pkt *msg_pkt;
static struct pkt *ack_pkt;
static struct msg buffer[MSG_SIZE];
static struct msg buffer_B[MSG_SIZE];


int checksum(char *str){
    int sum = 0;
    int j = 0;
    while (j < 20) {
        sum += str[j];
        j++;
    }
    return sum;
}

void make_pkt(int seq, (struct msg)*message, int checksum){
    msg_pkt->seqnum = seq;
    msg_pkt->acknum = seq;
    strcpy(msg_pkt->payload, message.data);
    msg_pkt->checksum = checksum + msg_pkt->seqnum + msg_pkt->acknum;
}

void make_ack(int seq, int checksum){
    ack_pkt->seqnum = seq;
    ack_pkt->acknum = seq;
    ack_pkt->checksum = ack_pkt->seqnum + ack_pkt->acknum;
}

/* called from layer 5, passed the data to be sent to other side */
void A_output(message)
  struct msg message;
{
    int check_msg;
    if(msgc > MSG_SIZE - 1){
        exit(-1);
    }
    buffer[msgc] = messages;
    msgc++;
    while(next_seq < base + N){
        if(next_seq == base)
            starttimer(0, increment);
        check_msg = checksum(buffer[next_seq].data);
        make_pkt(next_seq, buffer[next_seq].data, check_msg);
        tolayer3(0, (struct pkg)*msg_pkt);
        next_seq++;
    }

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{

}

/* called when A's timer goes off */
void A_timerinterrupt()
{

}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
  struct pkt packet;
{

}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{

}
