struct auth_response {
    string auth_token<16>;
    int status;
};

struct approve_request {
    string auth_token<>;
    string signature<>;
    string permissions<>;
};

struct access_request {
    string id<>;
    string signature<>;
    string auth_token<>;
    int refresh_state;
};

struct access_response {
    string access_token<>;
    string refresh_token<>;
    int ttl;
};

struct action_request {
    string action<>;
    string access_token<>;
    string resource<>;
};

struct refresh_response {
    string access_token<>;
    string refresh_token<>;
    int ttl;
};

program OAUTH_PROG {
    version OAUTH_VERS {
        struct auth_response REQUEST_AUTH(string) = 1;
        struct approve_request APPROVE_REQUEST_TOKEN(struct approve_request) = 2;
        struct access_response REQUEST_ACCESS(struct access_request) = 3;
        int VALIDATE_DELEGATED_ACTION(struct action_request) = 4;
        struct refresh_response REFRESH_SESSION(string) = 5;
    } = 1;
} = 0x30000005;