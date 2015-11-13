#define OCTECT 				"octet"
#define DNS_HEADER_LEN		12
#define DNS_QUESTION_LEN	13

#define RECURSIVE_DNS		256
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
	unsigned char	qlen;
	unsigned char*	domain;
	unsigned short	type;
	unsigned short 	qclass;
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

struct dnsQuestion* dnsQuestionCreator(int qlen){
	struct dnsQuestion* p = malloc(DNS_QUESTION_LEN);
	if(p != NULL){
		memset(p, 0, qlen);
		p->qlen = qlen;
		p->domain = malloc(qlen);
		return p;
	}
	else
	{
		perror("Question :(");
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

int parseDNS(int inBytes, unsigned char* buffer){
	return 0;
}

int sendDNS(int sock_udp, struct sockaddr_in* serverAddr, char* url){

	int questionLen;

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

	questionLen = strlen(url);

	question = dnsQuestionCreator(questionLen);
	memcpy(question->domain, url, questionLen + 1);

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

	int hi = sendto(sock_udp, msg, DNS_HEADER_LEN + questionLen + 6, 0, (struct sockaddr*)serverAddr, sizeof(*serverAddr));
	if(hi <= 0)
		perror("sendto");

	printf("Sent: %d bytes\n", hi);

	free(msg);

	return hi;
}

