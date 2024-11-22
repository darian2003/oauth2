/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "oauth.h"
#include "token.h"


struct auth_response *
request_auth_1_svc(char **argp, struct svc_req *rqstp)
{
	static struct auth_response result;
	char *id_to_authorize = *argp;
	int found = 0;

	/*
	 * insert server code here
	 */

	printf("BEGIN %s AUTHZ\n", id_to_authorize);

	result.auth_token = malloc(16);

	for(int i = 0; i < nr_users; i++) {
		if (strcmp(ids[i], id_to_authorize) == 0) {
			result.status = 200;
			char *auth_token = generate_access_token(id_to_authorize);
			strcpy(result.auth_token, auth_token);
			strcpy(auth_tokens[i], auth_token); // Save the auth token for future verification
			found = 1;
			printf("  RequestToken = %s\n", result.auth_token);
			break;
		}
	}

	if (!found) {
		result.status = 404;
	}

	//printf("returning\n");
	return &result;
}

struct approve_request *
approve_request_token_1_svc(struct approve_request *approve_request, struct svc_req *rqstp)
{
	static struct approve_request approve_response;

	/*
	 * insert server code here
	 */
	approve_response.auth_token = malloc(16);
	approve_response.signature = malloc(16);
	approve_response.permissions = malloc(100);
	strcpy(approve_response.auth_token, approve_request->auth_token);
	if (approvals[crt_approval][0] == '*') {
		// TODO
		memset(approve_response.signature, 0, 16);
		memset(approve_response.permissions, 0, 16);
	} else {
		char *signature = generate_signature_token(approve_request->auth_token);
		strcpy(approve_response.signature, signature);
		strcpy(approve_response.permissions, approvals[crt_approval]);

		// Save the signature for future verification. Save the permissions
		for (int i = 0; i < nr_users; i++) {
			if (strcmp(approve_request->auth_token, auth_tokens[i]) == 0) {
				strcpy(signatures[i], signature);
				strcpy(permissions[i], approve_response.permissions);
				break;
			}
		}
	}

	//printf("returning 2\n");
	crt_approval += 1;
	return &approve_response;
}

struct access_response *
request_access_1_svc(struct access_request *access_request, struct svc_req *rqstp)
{
	static struct access_response  access_response;
	access_response.access_token = malloc(16);

	/*
	 * insert server code here
	 */
	for (int i = 0; i < nr_users; i++) {
		if (strcmp(ids[i], access_request->id) == 0) { // User found	 
			if (strcmp(signatures[i], access_request->signature) != 0) {
				return NULL;
			} else {
				char *access_token = generate_access_token(access_request->auth_token);
				strcpy(access_response.access_token, access_token);
				access_response.ttl = ttl;
				strcpy(access_tokens[i], access_token);
				printf("  AccessToken = %s\n", access_token);
				break;
			}
		}
	}

	//printf("returning 3\n");
	return &access_response;
}

int *
validate_delegated_action_1_svc(struct access_request *argp, struct svc_req *rqstp)
{
	static int  result;

	/*
	 * insert server code here
	 */



	return &result;
}
