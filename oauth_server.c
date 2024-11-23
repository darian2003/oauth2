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

	printf("BEGIN %s AUTHZ\n", id_to_authorize);
	fflush(stdout);

	result.auth_token = malloc(16);

	// Search the id in the database.
	for(int i = 0; i < nr_users; i++) {
		if (strcmp(ids[i], id_to_authorize) == 0) {
			result.status = 200;

			// Create AUTH token.
			char *auth_token = generate_access_token(id_to_authorize);
			strcpy(result.auth_token, auth_token);

			// Save the auth token for future verification.
			strcpy(auth_tokens[i], auth_token); 
			found = 1;
			printf("  RequestToken = %s\n", result.auth_token);
			fflush(stdout);
			break;
		}
	}

	if (!found) {
		result.status = 404;
	}

	return &result;
}

struct approve_request *
approve_request_token_1_svc(struct approve_request *approve_request, struct svc_req *rqstp)
{
	static struct approve_request approve_response;

	approve_response.auth_token = malloc(16);
	approve_response.signature = malloc(16);
	approve_response.permissions = malloc(100);
	strcpy(approve_response.auth_token, approve_request->auth_token);

	// Check if the request was approved by the user.
	if (approvals[crt_approval][0] == '*') {
		memset(approve_response.signature, 0, 16);
		memset(approve_response.permissions, 0, 16);
	} else {

		// User approved AUTH request. Generating signature.
		char *signature = generate_signature_token(approve_request->auth_token);
		strcpy(approve_response.signature, signature);
		strcpy(approve_response.permissions, approvals[crt_approval]);

		// Find the user by its AUTH token. Save the signature for future verification. Save token's permissions.
		for (int i = 0; i < nr_users; i++) {
			if (strcmp(approve_request->auth_token, auth_tokens[i]) == 0) {
				strcpy(signatures[i], signature);
				strcpy(permissions[i], approve_response.permissions);
				break;
			}
		}
	}

	// Move to the next approval entry.
	crt_approval += 1;
	return &approve_response;
}

struct access_response *
request_access_1_svc(struct access_request *access_request, struct svc_req *rqstp)
{
	static struct access_response  access_response;
	access_response.access_token = malloc(16);
	access_response.refresh_token = calloc(16, 1);

	// Find the user by id.
	for (int i = 0; i < nr_users; i++) {
		if (strcmp(ids[i], access_request->id) == 0) { // User found.

			// Invalid signature.
			if (strcmp(signatures[i], access_request->signature) != 0) {
				return NULL;
			} else {

				// Generate access token.
				char *access_token = generate_access_token(access_request->auth_token);
				strcpy(access_response.access_token, access_token);

				// Save access token in server's database.
				strcpy(access_tokens[i], access_token); 
				printf("  AccessToken = %s\n", access_token);
				fflush(stdout);

				// Check if refresh token is requested.
				char *refresh_token;
				if (access_request->refresh_state) {
					refresh_token = generate_access_token(access_token);
					strcpy(access_response.refresh_token, refresh_token);

					// Save refresh token in server's memory.
					strcpy(refresh_tokens[i], refresh_token); 
					printf("  RefreshToken = %s\n", refresh_token);
					fflush(stdout);
				}
				
				// Update token's valability.
				access_response.ttl = ttl;
				ttls[i] = ttl;
				
				break;
			}
		}
	}

	return &access_response;
}

struct refresh_response *
refresh_session_1_svc(char **argp, struct svc_req *rqstp)
{
	static struct refresh_response  refresh_response;
	char *refresh_token = *argp;
	refresh_response.access_token = malloc(16);
	refresh_response.refresh_token = malloc(16);
	refresh_response.ttl = ttl;

	// Generate new access and refresh tokens.
	char *new_access_token = generate_access_token(refresh_token);
	char *new_refresh_token = generate_access_token(new_access_token);

	strcpy(refresh_response.access_token, new_access_token);
	strcpy(refresh_response.refresh_token, new_refresh_token);

	// Find user by previous refresh_token, save the new tokens and renew TTL.
	for (int i = 0; i < nr_users; i++) {
		if (!strcmp(refresh_tokens[i], refresh_token)) {

			strcpy(access_tokens[i], new_access_token);
			strcpy(refresh_tokens[i], new_refresh_token);
			ttls[i] = ttl;

			printf("BEGIN %s AUTHZ REFRESH\n", ids[i]);
			printf("  AccessToken = %s\n", new_access_token);
			printf("  RefreshToken = %s\n", new_refresh_token);
			fflush(stdout);

			break;
		}
	}

	return &refresh_response;
}

// Returns 0 if token does not have permissions for operation on the resource_searched and 1 if it has
int is_allowed(char *permissions, char operation, char *resource_searched) {

	// Create permission copy (strtok would have broken the original string).
	char *permission_copy = malloc(100);
	strcpy(permission_copy, permissions);

	// Iterate through each resource and its permissions.
	char *resource = strtok(permission_copy, ",\n");
	char *permission = strtok(NULL, ",\n");

	while (resource) {

		// Searching for the wanted resource.
		if (strcmp(resource, resource_searched) == 0) {
			for (int c = 0; c < strlen(permission); c++) {
				if (operation == permission[c]) { 
					// Permission found.
					free(permission_copy);
					return 1;
				}
			}
			break;
		}
		resource = strtok(NULL, ",\n");
		permission = strtok(NULL, ",\n");
	}

	free(permission_copy);
	return 0;
}

// Returns 1 if file exists in the system and 0 if it doesn't.
int file_exists(char *file_name) {
	for (int i = 0; i < nr_resources; i++) {
		if (!strcmp(resources[i], file_name))
			return 1;
	}
	return 0;
}

int *
validate_delegated_action_1_svc(struct action_request *argp, struct svc_req *rqstp)
{
	static int  result;

	int id = -1;
	int found = -1;
	char *token = calloc(16, 1);

	for (int i = 0; i < nr_users; i++) {

		// Find the user by access token.
		if (strlen(access_tokens[i]) && strcmp(argp->access_token, access_tokens[i]) == 0) {

			id = i;
			found = 0;

			// Decrease ttl.
			if (ttls[i] == 0) {
				result = TOKEN_EXPIRED;
				break;
			}

			found = 1;
			ttls[i] -= 1;

			if (!file_exists(argp->resource)) {
				result = RESOURCE_NOT_FOUND;
				break;
			}

			// Get operation type.
			char operation;
			if (!strcmp(argp->action, "EXECUTE")) {
				operation = 'X';
			} else if (!strcmp(argp->action, "READ")) {
				operation = 'R';
			} else if (!strcmp(argp->action, "MODIFY")) {
				operation = 'M';
			} else if (!strcmp(argp->action, "DELETE")) {
				operation = 'D';
			} else if (!strcmp(argp->action, "INSERT")) {
				operation = 'I';
			} else {
				result = OPERATION_NOT_PERMITTED;
				break;
			}
			
			// Search for the resource and its permissions.
			int allowed = is_allowed(permissions[i], operation, argp->resource);

			if (!allowed) {
				result = OPERATION_NOT_PERMITTED;
				break;
			}

			result = PERMISSION_GRANTED;
			break;
		}

	}

	if (found < 0) {
		// Access token does not exist.
		result = PERMISSION_DENIED;
	} else if (found > 0) {
		strcpy(token, access_tokens[id]);
	}

	if (result == PERMISSION_GRANTED) {
		printf("PERMIT ");
		fflush(stdout);
	} else {
		printf("DENY ");
		fflush(stdout);
	}

	char *username = calloc(16,1);
	printf("(%s,%s,%s,%d)\n", argp->action, argp->resource, token, ttls[id]);
	fflush(stdout);

	return &result;
}
