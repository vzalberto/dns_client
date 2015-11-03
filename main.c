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

		printf("\n\nQuestion domain pointer size: %lu\n", sizeof(question->domain));

		memcpy(question->domain, argv[2], questionLen + 1);

		question->type = htons(1);
		question->qclass = htons(1);

	/*
		Send Message
	*/	
		unsigned char* msg = malloc(700);
		unsigned char* aux = malloc(DNS_HEADER_LEN + DNS_QUESTION_LEN);

		memcpy(msg, header, DNS_HEADER_LEN);
		memset(msg + DNS_HEADER_LEN, question->qlen, 1);
		memcpy(msg + DNS_HEADER_LEN + 1, question->domain, question->qlen);
		memset(msg + DNS_HEADER_LEN + 1 + questionLen, 0, 1);
		memcpy(msg + DNS_HEADER_LEN + 2 + questionLen, &question->type, 4);


	int hi = sendto(sock_udp, msg, DNS_HEADER_LEN + questionLen + 6, 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(hi <= 0)
		perror("sendto");

	printf("Sent: %d bytes\n", hi);

	//free(header);
	//free(question);
	free(msg);
	free(aux);
	close(sock_udp);

	return 0;
}