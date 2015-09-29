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
	unsigned char	   iph_tos:8;
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
	unsigned char	   icmp_type:8;
	unsigned char	   icmp_code:8;
	unsigned short int icmp_chksum:16;
	unsigned short int icmp_ident:16;
	unsigned short int icmp_sequence:16;
	/*PAYLOAD*/
};

unsigned short csum(unsigned short *buf, int len)
{
        unsigned long sum;
        for(sum=0; len>0; len--)
                sum += *buf++;
        sum = (sum >> 16) + (sum &0xffff);
        sum += (sum >> 16);
        return (unsigned short)(~sum);
}


int main(int argc, char *argv[]){

	int sd;
	char buffer[84]; // You can change the buffer size
	struct sockaddr_in sin;
	struct iphard *ip = (struct iphard *) buffer;
	struct icmphard *icmp = (struct icmphard *)(buffer + sizeof(struct iphard));
	int one = 1;
	const int *val = &one;
	int i;
	memset(buffer, 0, sizeof(buffer));


/* Create a raw socket with IP protocol. The IPPROTO_RAW parameter
* tells the sytem that the IP header is already included;
* this prevents the OS from adding another IP header. */

	sd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if(sd < 0){
		perror("socket() error"); 
		exit(-1);
	}
	
/* This data structure is needed when sending the packets
* using sockets. Normally, we need to fill out several
* fields, but for raw sockets, we only need to fill out
* this one field */
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(argv[1]);
	
// Here you can construct the IP packet using buffer[]
// - construct the IP header ...
// - construct the TCP/UDP/ICMP header ...
// - fill in the data part if needed ...
// Note: you should pay attention to the network/host byte order.

/* HERE IS WHERE I AM EDITTING TO MAKE PROPER SPOOF*/

	ip->iph_ver = 4;
	ip->iph_ihl = 0;
	//ip->iph_tos = '0';
	ip->iph_len = htons(84);	
	ip->iph_ident = htons(1337);
	ip->iph_ttl = 64;
	ip->iph_protocol = '1';
	//ip->iph_chksum = csum((unsigned short *) buffer, (sizeof(struct iphard) + sizeof(struct icmphard)));
	ip->iph_chksum = 0;
	//ip->iph_sourceip = inet_pton(AF_INET, argv[1], &(sin.sin_addr));
	//ip->iph_destip = inet_pton(AF_INET, argv[2], &(sin.sin_addr));

	ip->iph_sourceip = inet_addr(argv[1]);
	ip->iph_destip = inet_addr(argv[2]);


	if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one) < 0)) {
		perror("socketopt error");
		exit(-1);
	}

/* Send out the IP packet.
* ip_len is the actual size of the packet. */
	//if(sendto(sd, buffer, ip->iph_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0){
	//	perror("sendto() errorbaaa");
	//	exit(-1);
	//}
	
	for(i=0;i<84;i++){
		printf("Value: %c\n", buffer[i]);
	} 
}
