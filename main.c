#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "constants.h"

struct dnsHeader* dnsStdQueryHeader(){
	struct dnsHeader* p = malloc(DNS_HEADER_LEN);
	if(p != NULL){
		memset(p, 0, DNS_HEADER_LEN);
		return p;
	}
	else{
		perror(":(");
		exit(-1);
		return NULL;
	}
}

int main(int argc, char** argv){
	int sock_udp;
	struct dnsHeader* header;

	sock_udp = socket(AF_INET, SOCK_DGRAM, htons(17));

	header = dnsStdQueryHeader();

	//sendto(sock_udp, header, DNS_HEADER_LEN + questionLen, 0, dnsServer, sizeof(dnsServer));
	printf("Pécoro %x\n", (unsigned int)header);

	free(header);
	close(sock_udp);
}