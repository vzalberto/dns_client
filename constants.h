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
	unsigned int z:3; 
	unsigned int rcode:4;
	
	unsigned short qd;
	unsigned short an;
	unsigned short ns;
	unsigned short ar;

};

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

