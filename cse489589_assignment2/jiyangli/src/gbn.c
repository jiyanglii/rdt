#include "../include/simulator.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <getopt.h>

#define MSG_SIZE 1000  /* maximum number of messages can buffer */
#define TRAVEL_TIME 20

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
static int    N;
static int    msgc;
static int    seq;
static int    exp_seq;
static int    next_seq;
static int    base;
static int    count;
//static int    timer;

static struct pkt msg_pkt[MSG_SIZE];
static struct pkt ack_pkt;
static struct msg buffer[MSG_SIZE];

int checksum(char *str){
    int sum = 0;
    int j = 0;
    while (j < 20) {
        sum += str[j];
        j++;
    }
    return sum;
}

//seq means the value which need to trans
//next means the value need to retrans
void make_pkt(int seq, struct msg message, int check_sum, int next){
    msg_pkt[next].seqnum = seq;
    msg_pkt[next].acknum = seq;
    strcpy(msg_pkt[next].payload, message.data);
    msg_pkt[next].checksum = check_sum + msg_pkt[next].seqnum + msg_pkt[next].acknum;
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
    int a_check_msg;
    if(msgc > MSG_SIZE - 1){
        exit(-1);
    }
    
    buffer[msgc] = message;
    msgc++;
    
    if(next_seq < base + N){
        a_check_msg = checksum(buffer[next_seq].data);
        make_pkt(next_seq, buffer[next_seq], a_check_msg, next_seq);
        tolayer3(0, msg_pkt[next_seq]);
        if(next_seq == base) {
            starttimer(0,TRAVEL_TIME);
        }
        next_seq++;
        printf("the value of next_seq is: %d\n",next_seq);
    }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
struct pkt packet;
{
    int check_ack;
    check_ack = packet.seqnum + packet.acknum;
    
    if(check_ack == packet.checksum){
        if((packet.acknum >= base)&&(packet.acknum < next_seq)) {
            base = packet.acknum;
            stoptimer(0);
            starttimer(0, TRAVEL_TIME);
        }
    }
    printf("the value of base is: %d\n",base);
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
    
    for(int i = base; i < next_seq; i++) {
        tolayer3(0, msg_pkt[i]);
    }
    starttimer(0, TRAVEL_TIME);
    
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
    msgc = 0;
    seq = 0;
    base = 0;
    next_seq = 0;
    N = getwinsize();
    
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
struct pkt packet;
{
    printf("the received packet number is:%d", packet.seqnum);
    int b_check_msg;
    b_check_msg = packet.seqnum + packet.acknum + checksum(packet.payload);
    
    if(b_check_msg == packet.checksum) {
        if((packet.seqnum >= exp_seq)&&(packet.seqnum < exp_seq + N)) {
            make_ack(exp_seq,b_check_msg);
            tolayer3(1, ack_pkt);
            
            if(packet.seqnum == exp_seq) {
                tolayer5(1,packet.payload);
                exp_seq++;
            }
        }
        else{
            make_ack(exp_seq,b_check_msg);
            tolayer3(1, ack_pkt);
        }
    }
    printf("the value of exp_seq is: %d\n",exp_seq);
}

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
    int exp_seq = 0;
}
