#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct dnsQuestion{
	unsigned short	qlen;
	unsigned char*	qname;
	unsigned short	type;
	unsigned short 	qclass;
};

unsigned short parseLabels(char* url, unsigned char* qnameAddr){
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

	qnameAddr = qname;
	return total_bytes;
}


int main(){
	struct dnsQuestion* question = malloc(16);

	question->qlen = parseLabels("www.escom.ipn.mx", question->qname);
	question->type = htons(1);
	question->qclass = htons(1);

	free(question);

	return 0;
}