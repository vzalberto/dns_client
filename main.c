#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "constants.h"

int main(int argc, char** argv){
	int sock_udp, s, questionLen;

	unsigned char buf[sizeof(struct in_addr)];
	unsigned char* msg;
	
	struct dnsHeader* 	header;
	struct dnsQuestion*	question;
	struct sockaddr_in 	serverAddr;

	s = inet_pton(AF_INET, argv[1], buf);
        if (s <= 0) {
    	    if (s == 0)
        		   fprintf(stderr, "Not in presentation format\n");
               else
                   perror("inet_pton");
               exit(EXIT_FAILURE);
           }

    memcpy(&serverAddr.sin_addr, buf, 4);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(53);

	sock_udp = socket(AF_INET, SOCK_DGRAM, 0);

	/*
		Build Header
	*/

	header = dnsStdQueryHeader();

	header->qd 		= 1;
	header->flags 	= RECURSIVE_DNS;

	/*

		Build Question

	*/

		questionLen = strlen(argv[2]);

		question->qlen = questionLen;
		question->domain = malloc(questionLen);

		memcpy(question->domain, argv[2], questionLen);
		question->type = 1;
		question->qclass = 1;

	/*
		Send Message
	*/	
		unsigned char* aux;

		memcpy(msg, header, DNS_HEADER_LEN);
		aux = memset(msg + DNS_HEADER_LEN, question->qlen, 1);
		memcpy(aux, question->domain, question->qlen);
		memset(aux + question->qlen, 0, 1);
		memcpy(aux + question->qlen + 1, &question->type, 4);


	int hi = sendto(sock_udp, msg, DNS_HEADER_LEN + questionLen + 6, 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(hi <= 0)
		perror("sendto");

	printf("Sent: %d bytes\n", hi);

	free(header);
	close(sock_udp);

	return 0;
}