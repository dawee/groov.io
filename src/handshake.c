#include "groov.h"

#include <stdio.h>
#include <string.h>

static groov_config_t groov_config;

/*
 * Request
 */

static const char * handshake_request_header = "GET /socket.io/?transport=websocket HTTP/1.1";
static const char * handshake_request_connection = "Connection: Upgrade";
static const char * handshake_request_upgrade = "Upgrade: websocket";

/*
 * Response
 */

static int received = 0;
static char buf_base[GROOV_MAX_HANDSHAKE_SIZE];
static uv_buf_t buf = {.base = buf_base};


static int groov_handshake__equals_at(unsigned index, char byte) {
  return buf.len > index && buf.base[index] == byte;
}

static int groov_handshake__is_new_line_at(unsigned index) {
  return groov_handshake__equals_at(index, 13) && groov_handshake__equals_at(index + 1, 10);
}

static int groov_handshake__ended() {
  return buf.len >= 4
    && groov_handshake__is_new_line_at(buf.len - 4)
    && groov_handshake__is_new_line_at(buf.len - 2);
}

void groov_init_handshake(groov_config_t * config) {
  groov_config = *config;
}

void groov_reset_handshake_parser() {
  received = 0;
  buf.len = 0;
}

void groov_stream_to_handshake_parser(char byte) {
  if (received || buf.len == GROOV_MAX_HANDSHAKE_SIZE) return;

  buf.base[buf.len] = byte;
  buf.len++;

  if (groov_handshake__ended()) {
    received = 1;
    groov_write_incoming_handshake_event();
  }
}

void groov_serialize_handshake_request(groov_message_event_t * message) {
  sprintf(message->base,
    "%s\n%s\n%s\nHost: %s:%d\nSec-WebSocket-Version: %d\nSec-WebSocket-Key:%s\n\n",
    handshake_request_header,
    handshake_request_connection,
    handshake_request_upgrade,
    groov_config.host_name,
    groov_config.host_port,
    GROOV_PROTOCOL_VERSION,
    "MTMtMTQ2OTcwMDczOTcwNA=="
  );

  message->len = strlen(message->base);
}