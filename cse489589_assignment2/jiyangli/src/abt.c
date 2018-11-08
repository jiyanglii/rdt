#include "../include/simulator.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <getopt.h>

#define MSG_SIZE 1000
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
static int msgc = 0;
static int seq = 0;
static int exp_seq = 0;
static int stop_wait;
static int count_msg;
static int N;
static float increment = 15;
static struct pkt msg_pkt;
static struct pkt ack_pkt;
static struct msg buffer[MSG_SIZE];

int checksum(char *str){
    int sum = 0;
    int j = 0;
    while (j < 20) {
        sum += str[j];
        j++;
    }
//    while (sum >> 16) {
//        sum = (sum & 0xFFFF) + (sum >> 16);
//    }
    return sum;
}

void make_pkt(int seq, struct msg message, int check_sum){
    msg_pkt.seqnum = seq;
    msg_pkt.acknum = seq;
    strcpy(msg_pkt.payload, message.data);
    msg_pkt.checksum = check_sum + msg_pkt.seqnum + msg_pkt.acknum;
}

void make_ack(int seq, int check_sum){
    ack_pkt.seqnum = seq;
    ack_pkt.acknum = seq;
    ack_pkt.checksum = ack_pkt.seqnum + ack_pkt.acknum;
}

/* called from layer 5, passed the data to be sent to other side */
void A_output(message)
  struct msg message;
{
    int check_msg;

    if(msgc > MSG_SIZE - 1){
        exit(-1);
    }
    buffer[msgc] = message;
    msgc++;

    if(stop_wait == 0){
        check_msg = checksum(message.data);
        make_pkt(seq, buffer[count_msg], check_msg);
        printf("check packet sent: %d\n", count_msg);
        tolayer3(0, msg_pkt);
        starttimer(0, increment);
        stop_wait = 1;
    }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{
    int check_ack;
    check_ack = packet.seqnum + packet.acknum;
    if(check_ack == packet.checksum){
        if(packet.acknum == seq){
            seq = (seq + 1) % 2;
            stoptimer(0);
            printf("check packet received: %d\n", count_msg);
            count_msg++;
        }
    }
    stop_wait = 0;
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
    stop_wait = 0;
    
    tolayer3(0, msg_pkt);
//    seq = (seq + 1) % 2;
    starttimer(0, increment);
    stop_wait = 1;
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
    msgc = 0;
    seq = 0;
    count_msg = 0;
    stop_wait = 0;
    increment = 20;
    N = getwinsize();

}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
  struct pkt packet;
{
    int check_msg;
    check_msg = packet.seqnum + packet.acknum + checksum(packet.payload);
    if(check_msg == packet.checksum){
        make_ack(packet.seqnum, check_msg);
        tolayer3(1, ack_pkt);
        if(packet.seqnum == exp_seq){
            tolayer5(1, packet.payload);
            exp_seq = (exp_seq + 1) % 2;
        }
    }
}

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
    exp_seq = 0;
}
