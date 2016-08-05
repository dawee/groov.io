#include "groov.h"

typedef enum {
  RESERVED,
  OPCODE,
  MASK,
  PAYLOAD_LEN,
  EXTENDED_PAYLOAD_LEN,
  PAYLOAD,
} groov_ws_packet__state_t;

static groov_ws_packet_t packet;
static groov_ws_packet__state_t packet_state;

void groov_reset_ws_packet_parser() {
  packet_state = RESERVED;
}

void groov_stream_to_ws_packet_parser(char byte) {
}