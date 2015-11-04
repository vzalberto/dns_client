#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"


int main(){
	struct dnsReply dns;

	dns.id = 7;

	dns.qr = 0;
	dns.op = 2;
	dns.aa = 0;
	dns.tc = 0;
	dns.rd = 1;
	dns.ra = 0;
	dns.z = 0;
	dns.rcode = 0;

	dns.qd = 2;
	dns.an = 0;
	dns.ns = 0;
	dns.ar = 8;

	short* p;

	p = (short*)&dns;

	printf("\nDNS: %d\n", *p);
		printf("\nDNS: %x\n", *(p+4));
	


}