#include "groov.h"

#include <stdio.h>
#include <string.h>

static char host_name[GROOV_MAX_HOST_NAME_SIZE];
static int host_port;

static const char * handshake_request_header = "GET /socket.io/?transport=websocket HTTP/1.1";
static const char * handshake_request_connection = "Connection: Upgrade";
static const char * handshake_request_upgrade = "Upgrade: websocket";


void groov_init_serialize(groov_config_t * config) {
  host_port = config->host_port;
  memcpy(host_name, config->host_name->base, config->host_name->len);
}

void groov_serialize_handshake_request(groov_message_event_t * message) {
  sprintf(message->base,
    "%s\n%s\n%s\nHost: %s:%d\nSec-WebSocket-Version: %d\nSec-WebSocket-Key:%s\n\n",
    handshake_request_header,
    handshake_request_connection,
    handshake_request_upgrade,
    host_name,
    host_port,
    GROOV_PROTOCOL_VERSION,
    "MTMtMTQ2OTcwMDczOTcwNA=="
  );

  message->len = strlen(message->base);
}
