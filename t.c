#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char* parseLabels(char* url){
	const char s[2] = ".";
	unsigned char* qname = malloc(1500);

	unsigned short total_bytes;
	unsigned short total_labels;

	unsigned char lenght;

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

	return qname;
}

int main(){

	parseLabels("www.escom.ipn.mx");
	//printf(labels);

/*
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
	*/

		return 0;
}