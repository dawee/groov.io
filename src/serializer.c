#include "serializer.h"
#include <stdio.h>

void io_serialize_handshake_request(uv_buf_t * buf, io_handshake_request_t * req) {
  sprintf(buf->base,
    "%s\nHost: %s:%d\nSec-WebSocket-Version: %d\nSec-WebSocket-Key:%s\n%s\n\n",
    "GET /socket.io/?EIO=3&transport=websocket HTTP/1.1\nConnection: Upgrade\nUpgrade: websocket",
    req->hostname,
    req->port,
    req->version,
    req->key,
    "Sec-WebSocket-Extensions: permessage-deflate; client_max_window_bits"
  );

  buf->len = 273;
}
