/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "oauth.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace std;


void
checkprog_1(char *host)
{
	CLIENT *clnt;
	struct auth_response  *result_1;
	char * request_auth_1_arg;

	printf("%d %s\n", strlen(request_auth_1_arg), request_auth_1_arg);

#ifndef	DEBUG
	clnt = clnt_create (host, CHECKPROG, CHECKVERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	result_1 = request_auth_1(&request_auth_1_arg, clnt);
	if (result_1 == (struct auth_response *) NULL) {
		clnt_perror (clnt, "call failed");
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;

	fflush(stdout);
	printf("dadadada\n\n");

	if (argc < 2) {
		printf ("usage: %s server_host operations_file\n", argv[0]);
		exit (1);
	} else if (argc < 3) {
		printf ("no operations file found\n")
		exit (1);
	}

	host = argv[1];
	char *operations_file = argv[2];

	fflush(stdout);
	printf("%s\n", operations_file);
	
	// ifstream file(operations_file);


	//checkprog_1 (host);

exit (0);
}