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
	struct dnsHeader* header;
	struct sockaddr_in serverAddr;

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

	questionLen = strlen(argv[2]);
	header = dnsStdQueryHeader(questionLen);
	memcpy(header + DNS_HEADER_LEN, argv[2], questionLen);

	printf("questionLen: %d\n", questionLen);

	memoryPrint((unsigned char*)header, DNS_HEADER_LEN + questionLen);

	int hi = sendto(sock_udp, header, DNS_HEADER_LEN + questionLen, 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(hi <= 0)
		perror("sendto");

	printf("Pécoro %x\nSent: %d\n", (unsigned int)header, hi);

	free(header);
	close(sock_udp);

	return 0;
}