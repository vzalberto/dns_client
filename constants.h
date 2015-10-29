#define OCTECT 				"octet"
#define DNS_HEADER_LEN		12
#define RECURSIVE_DNS		256
#define DNS_TYPE_A			1
#define DNS_QCLASS_IN		1
#define DNS_MAX_QUESTION	300

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
	unsigned char**	domain;
	unsigned short	type;
	unsigned short 	qclass;
};

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

void memoryPrint(unsigned char* start, int bytes){
	int i;
	for(i = 0; i <= bytes; i++){
		if(i % 8 == 0 && i != 0)
			printf("\n");
		printf("%x\t", *(start + i));
	}

	printf("\n");

}