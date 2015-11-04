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
	pid_t childPID;

	unsigned char buf[sizeof(struct in_addr)];
	unsigned char buffer[DNS_MAX_RESPONSE];
	
	struct dnsHeader* 	header;
	struct dnsQuestion*	question;
	struct sockaddr_in 	serverAddr;

	/*
	Aqui empieza lo chido
	*/

	sock_udp = socket(AF_INET, SOCK_DGRAM, 0);
	childPID = fork();

	if(childPID == 0){

	}
	else{
		int inBytes = -1;
		struct sockaddr reply_addr;

		n = (socklen_t) sizeof(reply_addr);

		while(inBytes < 0){
			inBytes = recvfrom(sock_udp, buffer, DNS_MAX_RESPONSE, 0, reply_addr, &n);

		}
	}

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


	/*
		Build Header
	*/

	printf("Building header...\n");
	header = dnsStdQueryHeader();

	header->id 		= 0;
	header->flags 	= htons(RECURSIVE_DNS);
	header->qd = htons(1);
	header->an = 0;
	header->ns = 0;
	header->ar = 0;

	/*

		Build Question

	*/

	printf("Building question...\n");

	printf("\nWhere is %s?\n", argv[2]);

	questionLen = strlen(argv[2]);

	question = dnsQuestionCreator(questionLen);
	memcpy(question->domain, argv[2], questionLen + 1);

	question->type = htons(1);
	question->qclass = htons(1);

	/*
		Send Message
	*/	
	unsigned char* msg = malloc(700);

	memcpy(msg, header, DNS_HEADER_LEN);
	memset(msg + DNS_HEADER_LEN, question->qlen, 1);
	memcpy(msg + DNS_HEADER_LEN + 1, question->domain, question->qlen);
	memset(msg + DNS_HEADER_LEN + 1 + questionLen, 0, 1);
	memcpy(msg + DNS_HEADER_LEN + 2 + questionLen, &question->type, 4);

	int hi = sendto(sock_udp, msg, DNS_HEADER_LEN + questionLen + 6, 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(hi <= 0)
		perror("sendto");

	printf("Sent: %d bytes\n", hi);

	free(msg);
	close(sock_udp);

	return 0;
}