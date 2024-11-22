# Parameters

CLIENT = oauth_client
SERVER = oauth_server

# List of previously generated files
SOURCES_CLNT.c = oauth_client.c oauth_clnt.c oauth_xdr.c
SOURCES_SVC.c = oauth_server.c oauth_svc.c oauth_xdr.c
SOURCES.h = oauth.h

OBJECTS_CLNT = $(SOURCES_CLNT.c:%.c=%.o)
OBJECTS_SVC = $(SOURCES_SVC.c:%.c=%.o)

# Compiler flags
CFLAGS += -g -I/usr/include/tirpc
LDLIBS += -lnsl -ltirpc

# Targets
all: $(CLIENT) $(SERVER)

$(CLIENT): $(OBJECTS_CLNT)
	$(LINK.c) -o $(CLIENT) $(OBJECTS_CLNT) $(LDLIBS)

$(SERVER): $(OBJECTS_SVC)
	$(LINK.c) -o $(SERVER) $(OBJECTS_SVC) $(LDLIBS)

clean:
	$(RM) core $(OBJECTS_CLNT) $(OBJECTS_SVC) $(CLIENT) $(SERVER)
