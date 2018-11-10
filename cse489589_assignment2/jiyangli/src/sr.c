#include "../include/simulator.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <getopt.h>

#define MSG_SIZE 1000  /* maximum number of messages can buffer */
//#define N 10           /* window size */

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
static int    msgc = 0;
static int    seq = 0;
static int    exp_seq = 0;
static int    next_seq = 0;
static int    base = 0;
static int    count = 0;
//static int    timer;
static float  increment = 20;
static double timevalue[MSG_SIZE];
static int    b_ack[MSG_SIZE*10];

static int    ackval[MSG_SIZE];
static struct pkt msg_pkt[MSG_SIZE];
static struct pkt ack_pkt;
static struct pkt rcv_pkt[MSG_SIZE*10];
static struct msg buffer[MSG_SIZE];

int timemin( ) {
    double tmp;
    int    index;
    index = 0;
    tmp   = timevalue[base];
    
    for(int i = 1; i < (next_seq - base); i++) {
        if(ackval[base+i] == 0) {
            index = i;
            tmp   = timevalue[base+i];
            break;
        }
    }
    
    for(int i = 1; i < (next_seq - base); i++) {
        if((ackval[base+i] == 0)&&(timevalue[base+i] < tmp)) {
            tmp   = timevalue[base+i];
            index = i;
        }
    }
    return index;
}

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
        
        timevalue[next_seq] = get_sim_time();
        ackval[next_seq] = 0;
        
        if(next_seq == base) {
            starttimer(0,increment);
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
            if(packet.acknum == base) {
                ackval[base] = 1;
                stoptimer(0);
                base++;
                
                while(base < next_seq) {
                    if(ackval[base] == 1) {
                        base++;
                    }
                    else{
                        int ind;
                        ind = timemin();
                        starttimer(0, increment - (get_sim_time() - timevalue[base+ind]));
                        break;
                    }
                }
            }
            else {
                ackval[packet.acknum] = 1;
            }
        }
    }
    printf("the value of base is: %d\n",base);
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
    int min_index, min_next;
    min_index = timemin();
    tolayer3(0, msg_pkt[base+min_index]);
    timevalue[base+min_index] = get_sim_time();
    //stoptimer(0);
    printf("timeout base %d\n", base);
    printf("timeout happened %d\n", base + min_index);
    
    min_next = timemin();
    printf("timeout happened the next time %d\n", base + min_next);
    starttimer(0, increment - (get_sim_time() - timevalue[base + min_next]));
    
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
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
            make_ack(packet.seqnum,b_check_msg);
            tolayer3(1, ack_pkt);
            strcpy(rcv_pkt[packet.seqnum].payload, packet.payload);
            
            if(packet.seqnum == exp_seq) {
                tolayer5(1,packet.payload);
                b_ack[exp_seq] = 1;
                exp_seq++;
                
                for(int i = 0; i< N; i++){
                    if(b_ack[exp_seq] == 1) {
                        tolayer5(1, rcv_pkt[exp_seq].payload);
                        exp_seq++;
                    }
                    else {
                        break;
                    }
                }
            }
            else {
                b_ack[packet.seqnum] = 1;
            }
        }
        else{
            make_ack(packet.seqnum,b_check_msg);
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
