#include "groov.h"


static int received = 0;
static char buf_base[GROOV_MAX_HANDSHAKE_SIZE];
static uv_buf_t buf = {.base = buf_base};


static int groov_handshake__equals_at(int index, char byte) {
  return buf.len > index && buf.base[index] == byte;
}

static int groov_handshake__is_new_line_at(int index) {
  return groov_handshake__equals_at(index, 13) && groov_handshake__equals_at(index + 1, 10);
}

static int groov_handshake__ended() {
  return buf.len >= 4
    && groov_handshake__is_new_line_at(buf.len - 4)
    && groov_handshake__is_new_line_at(buf.len - 2);
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
