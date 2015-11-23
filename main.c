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
	int sock_udp, s, n;
	pid_t childPID;

	unsigned char buf[sizeof(struct in_addr)];
	unsigned char buffer[1500];
	
	struct sockaddr_in 	serverAddr;

	/*
	Aqui empieza lo chido
	*/

	sock_udp = socket(AF_INET, SOCK_DGRAM, 0);
	childPID = fork();

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

	if(childPID == 0){
		sendDNS(sock_udp, &serverAddr, argv[2]);
	}
	else{
		int inBytes = -1;
		struct sockaddr reply_addr;

		n = sizeof(reply_addr);

		while(inBytes < 0){
			inBytes = recvfrom(sock_udp, buffer, DNS_MAX_RESPONSE, 0, &reply_addr, (socklen_t*)&n);
			printf("\nIn Bytes: %d\n", inBytes);
			printDNSmsg((struct dnsReply*)buffer);

		}
	}

	close(sock_udp);

	return 0;
}