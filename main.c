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
	int sock_udp, s, n, fd[2], sent_bytes, pipe_msg;
	pid_t childPID;

	unsigned char buf[sizeof(struct in_addr)];
	unsigned char buffer[1500];

	unsigned char* in_buf;
	unsigned char* out_buf;
	
	struct sockaddr_in 	serverAddr;

	/*
	Aqui empieza lo chido
	*/

	in_buf = NULL;
	out_buf = NULL;

	sock_udp = socket(AF_INET, SOCK_DGRAM, 0);
	childPID = fork();
	pipe(fd);

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

    /*Process handling*/

	if(childPID == 0){
		close(fd[0]);

		sent_bytes = sendDNS(sock_udp, &serverAddr, argv[2]);
		printf("\nSicierto o nocierto: %d", sent_bytes);
		memcpy(in_buf, &sent_bytes, sizeof(sent_bytes));

		write(fd[1], in_buf, sizeof(sent_bytes));

	}
	else{
		close(fd[1]);

		int inBytes = -1;
		struct sockaddr reply_addr;

		n = sizeof(reply_addr);

		while(inBytes < 0){
			inBytes = recvfrom(sock_udp, buffer, DNS_MAX_RESPONSE, 0, &reply_addr, (socklen_t*)&n);
			
			read(fd[0], out_buf, sizeof(sent_bytes));
			memcpy(&pipe_msg, out_buf, sizeof(sent_bytes));

			printf("\nSicierto: %d\n", pipe_msg);
			printDNSmsg((struct dnsReply*)buffer);

		}
	}

	close(sock_udp);

	return 0;
}