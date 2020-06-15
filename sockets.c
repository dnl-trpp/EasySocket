#include <stdio.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <linux/ip.h>

void error(char* msg);
void pkt_print(unsigned char * buffer,int buflen);

int pkt_counter=0;

int main(int argc,char* argv[]){
    int sockfd;
    unsigned char *buffer = (unsigned char *) malloc(65536); //to receive data
    memset(buffer,0,65536);
    struct sockaddr saddr;
    int saddr_len = sizeof (saddr);

    if(argc<2){
        printf("Usage: %s [count]\n",argv[0]);
        return 1;
    }

    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); //opens a raw socket for all packets
    if (sockfd<0) error("[!] Can't open socket");
    for(int i=0; i<atoi(argv[1]);i++){
        //Receive a network packet and copy in to buffer
        int buflen = recvfrom(sockfd,buffer,65536,0,&saddr,(socklen_t *)&saddr_len);
        if(buflen<0) error("[!] Can'r recive in buffer");
        //04:B4:29:40:FA:71

        pkt_print(buffer,buflen);
    
    }
    close(sockfd);
}



void error(char* msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

void pkt_print(unsigned char * buffer,int buflen){
    struct ethhdr *eth = (struct ethhdr *)(buffer);
    printf("Recived %d bytes from %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n",buflen,eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
    printf("Packet No: %d\n",pkt_counter);
    pkt_counter++;
    printf("======DATA LAYER======\n");
    printf("EtherType: %04x\n",ntohs(eth->h_proto));
    printf("Packet: ");
    for (int i=0;i< buflen;i++){
        printf("%02x ", buffer[i]);
    }
    if(ntohs(eth->h_proto)== ETH_P_IP){
        struct iphdr *ip = (struct ipdhr*)(buffer + ETH_HLEN);
        printf("\n");
        printf("\t======IP LAYER======\n");
        printf("\tSender ip: %s\n",inet_ntoa( *((struct in_addr*) &(ip->saddr) )));
        printf("\tDestination ip: %s\n",inet_ntoa( *((struct in_addr*) &(ip->daddr) )));
    
    }

    printf("\n\n");
    
}