#include "serializer.h"
#include <stdio.h>
#include <string.h>

void io_serialize_handshake_request(uv_buf_t * buf, io_handshake_request_t * req) {
  sprintf(buf->base,
    "%s\n%s\n%s\nHost: %s:%d\nSec-WebSocket-Version: %d\nSec-WebSocket-Key:%s\n\n",
    "GET /socket.io/?transport=websocket HTTP/1.1",
    "Connection: Upgrade",
    "Upgrade: websocket",
    req->hostname,
    req->port,
    req->version,
    req->key
  );

  buf->len = strlen(buf->base);
}
