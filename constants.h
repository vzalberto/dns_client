#define OCTECT 				"octet"
#define DNS_HEADER_LEN		12

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
