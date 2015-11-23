#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define OCTECT 				"octet"
#define DNS_HEADER_LEN		12
#define DNS_QUESTION_LEN	13

#define RECURSIVE_DNS		256
#define DNS_LABEL_SIZE		72
#define DNS_TYPE_A			1
#define DNS_QCLASS_IN		1
#define DNS_MAX_QUESTION	300
#define DNS_MAX_RESPONSE	1500

/*
	Flags:
	QR  Op    AA  TC  RD  RA  Z    RC
	0   0000  0   0   0   0   000  0000
*/

struct dnsHeader{
	unsigned short id;
	unsigned short flags;
	unsigned short qd;
	unsigned short an;
	unsigned short ns;
	unsigned short ar;
};

struct dnsQuestion{
	unsigned char*	qname;
	unsigned short	type;
	unsigned short 	qclass;	
	unsigned short	qlen;
};

struct dnsLabel{
	unsigned short	bytes;
	unsigned char*	name;
};

struct dnsReply{
	unsigned short id;

	unsigned int qr:1;
	unsigned int op:4;
	unsigned int aa:1;
	unsigned int tc:1;
	unsigned int rd:1;
	unsigned int ra:1;
	unsigned int z:4; 
	unsigned int rcode:1;
	
	unsigned short qd;
	unsigned short an;
	unsigned short ns;
	unsigned short ar;

	

};

void printDNSmsg(struct dnsReply* msg){
	unsigned short rcode;
	rcode = ntohs(msg->rcode);

	printf("\nDNS HEADER:\n\n");

	printf("ID:\t%x\n", ntohs(msg->id));

	if(msg->qr)
		printf("QR:\tReply (%d)\n", msg->qr);
	else
		printf("QR:\tRequest (%d)\n", msg->qr);


	printf("Opcode:\t");
	switch(msg->op){
		case 0:
			printf("Standard Query (0)\n");
			break;
		case 1:
			printf("Inverse Query(1)\n");
			break;
		case 2:
			printf("Server Status\n");
			break;
		default:
			printf(":(\n");
			break;
	}

	if(msg->aa)
		printf("AA:\tServer is authority\n");
	else
		printf("AA:\tServer is NOT authority\n");

	if(msg->tc)
		printf("TC:\tMessage is truncated\n");
	else
		printf("TC:\tMessage is NOT truncated\n");

	if(msg->rd)
		printf("RD:\tRecursion desired\n");
	else
		printf("RD:\tRecursion is not desired\n");

	if(msg->ra)
		printf("RA:\tRecursion available\n");
	else
		printf("RA:\tRecursion is not available\n");

	printf("Z(0):\tReserved\n");

	printf("RCODE (%d): \t", rcode);
	switch(rcode){

		case 0:
			printf("No error\n");
			break;
		case 1:
			printf("Format error\n");
			break;
		case 2:
			printf("Server failure\n");
			break;
		case 3:
			printf("Name error\n");
			break;
		case 4:
			printf("Not implemented\n");
			break;
		case 5:
			printf("Refused\n");
			break;
		default:
			printf("Unknown\n");
			break;	
	}

	printf("QDCOUNT:\t%d\n", ntohs(msg->qd));

	printf("ANCOUNT:\t%d\n", ntohs(msg->an));

	printf("NSCOUNT:\t%d\n", ntohs(msg->ns));

	printf("ARCOUNT:\t%d\n", ntohs(msg->ar));

}

struct dnsHeader* dnsStdQueryHeader(){							//should I reuse this function?
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

void memoryPrint(unsigned char* start, int bytes){
	int i;
	for(i = 0; i <= bytes; i++){
		if(i % 8 == 0 && i != 0)
			printf("\n");
		printf("%x\t", *(start + i));
	}

	printf("\n");

}

struct dnsLabel* parseLabels(char* url){
	const char s[2] = ".";
	unsigned char* qname = malloc(1500);

	unsigned short total_bytes;
	unsigned short total_labels;

	unsigned char lenght;

	struct dnsLabel* label;
	label = malloc(16);

	char* aux = malloc(1500);

	total_labels = 0;
	total_bytes	 = 0;
	aux = strtok(strdup(url), s);

	while(aux != NULL){
		lenght = (unsigned char)strlen(aux);

		memcpy(qname + total_bytes, &lenght, 1);
		memcpy(qname + total_bytes + 1, aux, lenght);

		total_labels++;
		total_bytes+=lenght;
		total_bytes++;
		aux = strtok(NULL, s);
	}

	memset(qname + total_bytes, 0x00, 1);

	free(aux);
	printf("qname: (parse) %s\n", qname);

	label->bytes = total_bytes+1;
	label->name = qname;

	return label;
}

struct dnsQuestion* buildQuestion(char* url){
	struct dnsQuestion* p = malloc(DNS_QUESTION_LEN);
	struct dnsLabel* label;
	if(p != NULL){

		label = parseLabels(url);

		p->qname = label->name;
		p->qlen = label->bytes;

		p->type = htons(1);
		p->qclass = htons(1);

		return p;
	}
	else
	{
		perror("Question :(\n");
			exit(-1);
			return NULL;
	}

}

int sendDNS(int sock_udp, struct sockaddr_in* serverAddr, char* url){

	int hi;

	struct dnsHeader* 	header;
	struct dnsQuestion*	question;

	/*
		Build Header
	*/

	header = dnsStdQueryHeader();

	header->id 		= htons(1000);
	header->flags 	= htons(RECURSIVE_DNS);
	header->qd = htons(1);
	header->an = 0;
	header->ns = 0;
	header->ar = 0;
	
	/*
		Build Question
	*/

	question = buildQuestion(url);


		// printf("\np.qlen: %d\n", question->qlen);
		// printf("\np.qname: %s\n", question->qname);
		// printf("\np.type: %d\n", ntohs(question->type));
		// printf("\np.qclass: %d\n", ntohs(question->qclass));

	/*
		Build Message
	*/	

	unsigned char* msg = malloc(700);

	//Se copia el encabezado
	memcpy(msg, header, DNS_HEADER_LEN);

	//Copy Qname

	memcpy(msg + DNS_HEADER_LEN, question->qname, question->qlen);

	//Copy Type & Class

	memcpy(msg + DNS_HEADER_LEN + question->qlen, &question->type, 4);


	/*
		Send Message
	*/

	hi = sendto(sock_udp, msg, (DNS_HEADER_LEN + question->qlen + 4), 0, (struct sockaddr*)serverAddr, sizeof(*serverAddr));
	if(hi <= 0)
		perror("sendto");

	printf("Sent: %d bytes\n", hi);

	free(msg);

	return hi;
}

