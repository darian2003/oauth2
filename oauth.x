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
};

struct access_response {
    string access_token<>;
    int ttl;
};

struct action_request {
    string action<>;
    string access_token<>;
    string resource<>;
};

program CHECKPROG {
    version CHECKVERS {
        struct auth_response REQUEST_AUTH(string) = 1;
        struct approve_request APPROVE_REQUEST_TOKEN(struct approve_request) = 2;
        struct access_response REQUEST_ACCESS(struct access_request) = 3;
        int VALIDATE_DELEGATED_ACTION(struct access_request) = 4;
    } = 1;
} = 0x30000005;