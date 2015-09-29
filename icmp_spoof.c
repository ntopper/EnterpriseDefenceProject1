#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

struct iphard{
        unsigned char      iph_ihl:4, /* Little-endian */
                           iph_ver:4;
        unsigned char      iph_tos:8;
        unsigned short int iph_len:16;
        unsigned short int iph_ident:16;
        unsigned char      iph_flags:3;
        unsigned short int iph_offset:13;
        unsigned char      iph_ttl:8;
        unsigned char      iph_protocol:8;
        unsigned short int iph_chksum:16;
        unsigned int       iph_sourceip:32;
        unsigned int       iph_destip:32;
        /*options*/
};

struct icmphard{
        unsigned char      icmp_type:8;
        unsigned char      icmp_code:8;
        unsigned short int icmp_chksum:16;
        unsigned short int icmp_ident:16;
        unsigned short int icmp_sequence:16;
        /*PAYLOAD*/
};

int main(int argc, char *argv[]){

        int sd;

        struct sockaddr_in sin;
        struct iphard *ip;
        struct icmphard *icmp;
        char buffer[84]; // You can change the buffer size

/* Create a raw socket with IP protocol. The IPPROTO_RAW parameter
* tells the sytem that the IP header is already included;
* this prevents the OS from adding another IP header. */
        ip = (struct iphard *) buffer;
        //icmp = (struct icmphard *) ip;
        icmp = (struct icmphard *) (buffer + sizeof(struct iphard));
        sd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

        int i;
        //for(i=0;i<84;i++){
        //      buffer[i] = '0';
        //}

        if(sd < 0){
                perror("socket() error");
                exit(-1);
        }

        const int on = 1;

        if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on) < 0)) {
                perror("socketopt error");
                exit(-1);
        }

/* This data structure is needed when sending the packets
* using sockets. Normally, we need to fill out several
* fields, but for raw sockets, we only need to fill out
* this one field */
        sin.sin_family = AF_INET;

// Here you can construct the IP packet using buffer[]
// - construct the IP header ...
// - construct the TCP/UDP/ICMP header ...
// - fill in the data part if needed ...
// Note: you should pay attention to the network/host byte order.

/* HERE IS WHERE I AM EDITTING TO MAKE PROPER SPOOF*/

        memset(buffer, 0, sizeof(buffer));
        ip->iph_ver = 4;
        ip->iph_ihl = 5;
        //ip->iph_tos = '0';
        ip->iph_len = htons(84);
        ip->iph_ident = htons(1337);


/* Send out the IP packet.
* ip_len is the actual size of the packet. */
        if((sendto(sd, buffer, sizeof(buffer), 0, (struct sockaddr *)&sin, sizeof(sin))) < 0){
                perror("sendto() error");
                exit(-1);
        }

        for(i=0;i<84;i++){
                printf("Value: %c\n", buffer[i]);
        }
}
