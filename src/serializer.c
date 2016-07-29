#include "serializer.h"
#include <stdio.h>
#include <string.h>

static const char * handshake_request_header = "GET /socket.io/?transport=websocket HTTP/1.1";
static const char * handshake_request_connection = "Connection: Upgrade";
static const char * handshake_request_upgrade = "Upgrade: websocket";

void io_serialize_handshake_request(uv_buf_t * buf, io_handshake_request_t * req) {
  sprintf(buf->base,
    "%s\n%s\n%s\nHost: %s:%d\nSec-WebSocket-Version: %d\nSec-WebSocket-Key:%s\n\n",
    handshake_request_header,
    handshake_request_connection,
    handshake_request_upgrade,
    req->hostname,
    req->port,
    req->version,
    req->key
  );

  buf->len = strlen(buf->base);
}
