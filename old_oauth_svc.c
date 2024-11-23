#include "oauth.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

char **resources;
char **ids;
char **approvals;
char **auth_tokens;
char **refresh_tokens;
char **signatures;
char **permissions;
int ttl = 0;
int nr_users = 0;
int nr_resources = 0;
int nr_approvals = 0;
int crt_approval = 0;

void create_database(char **argv) {
	char *users_file = argv[1];
	char *resources_file = argv[2];
	char *approvals_file = argv[3];
	ttl = atoi(argv[4]);
	
	resources = malloc(MAX_LINES * sizeof(char *));
	ids = malloc(MAX_LINES * sizeof(char *));
	approvals = malloc(MAX_LINES * sizeof(char *));
	auth_tokens = malloc(MAX_LINES * sizeof(char *));
	signatures = malloc(MAX_LINES * sizeof(char *));
	access_tokens = malloc(MAX_LINES * sizeof(char *));
	permissions = malloc(MAX_LINES * sizeof(char *));


	for (int i = 0; i < MAX_LINES; i++) {
		resources[i] = calloc(20, sizeof(char));
		ids[i] = calloc(16, sizeof(char));
		approvals[i] = calloc(100, sizeof(char));
		auth_tokens[i] = calloc(16, sizeof(char));
		signatures[i] = calloc(16, sizeof(char));
		access_tokens[i] = calloc(16, sizeof(char));
		permissions[i] = calloc(100, sizeof(char));
	}

	FILE *file = fopen(users_file, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[50];  // Buffer to store each line
	const char delimiters[] = ",\n";

	// Read every line
	fgets(line, sizeof(line), file);
	nr_users = atoi(line);
    for (int i = 0; i < nr_users; i++) {
		fgets(line, sizeof(line), file);
		line[strcspn(line, "\n")] = '\0';
		strcpy(ids[i], line);
	}

	fclose(file);

	file = fopen(resources_file, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

	// Read every line
	fgets(line, sizeof(line), file);
	nr_resources = atoi(line);
    for (int i = 0; i < nr_resources; i++) {
		fgets(line, sizeof(line), file);
		line[strcspn(line, "\n")] = '\0';
		strcpy(resources[i], line);
	}
	fclose(file);

	file = fopen(approvals_file, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

	// Read every line
	int i = 0;
	nr_approvals = 0;
	while (fgets(line, sizeof(line), file)) {
		line[strcspn(line, "\n")] = '\0';
		strcpy(approvals[i], line);
		i += 1;
	}
	nr_approvals = i;
	fclose(file);

}

static void
oauth_prog_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		char *request_auth_1_arg;
		struct approve_request approve_request_token_1_arg;
		struct access_request request_access_1_arg;
		struct access_request validate_delegated_action_1_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case REQUEST_AUTH:
		_xdr_argument = (xdrproc_t) xdr_wrapstring;
		_xdr_result = (xdrproc_t) xdr_auth_response;
		local = (char *(*)(char *, struct svc_req *)) request_auth_1_svc;
		break;

	case APPROVE_REQUEST_TOKEN:
		_xdr_argument = (xdrproc_t) xdr_approve_request;
		_xdr_result = (xdrproc_t) xdr_approve_request;
		local = (char *(*)(char *, struct svc_req *)) approve_request_token_1_svc;
		break;

	case REQUEST_ACCESS:
		_xdr_argument = (xdrproc_t) xdr_access_request;
		_xdr_result = (xdrproc_t) xdr_access_response;
		local = (char *(*)(char *, struct svc_req *)) request_access_1_svc;
		break;

	case VALIDATE_DELEGATED_ACTION:
		_xdr_argument = (xdrproc_t) xdr_access_request;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (char *(*)(char *, struct svc_req *)) validate_delegated_action_1_svc;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	return;
}

int
main (int argc, char **argv)
{
	register SVCXPRT *transp;

	pmap_unset (OAUTH_PROG, OAUTH_VERS);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, OAUTH_PROG, OAUTH_VERS, oauth_prog_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (OAUTH_PROG, OAUTH_VERS, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, OAUTH_PROG, OAUTH_VERS, oauth_prog_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (OAUTH_PROG, OAUTH_VERS, tcp).");
		exit(1);
	}

	fflush(stdout);
	if (argc < 4) {
		printf ("usage: %s users_file resources_file approvals_file time_to_live\n", argv[0]);
		exit (1);
	}
	create_database(argv);

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}
