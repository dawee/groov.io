#include "groov.h"

typedef enum {
  OPCODE,
  PAYLOAD_LEN,
  EXTENDED_PAYLOAD_LEN,
  PAYLOAD,
} groov_ws_packet__state_t;

static groov_ws_packet_t packet;
static groov_ws_packet__state_t packet_state;
static unsigned long cursor;
static unsigned extended_bytes_count;

static void groov_ws_packet__switch_state(groov_ws_packet__state_t new_state) {
  packet_state = new_state;
  cursor = 0;
}

static void groov_ws_packet__parse_opcode(char byte) {
  packet.opcode = byte & 0x00001111;
  groov_ws_packet__switch_state(PAYLOAD_LEN);
}

static void groov_ws_packet__parse_payload_len(char byte) {
  packet.len = (uint64_t) (byte & 0x01111111);

  if (packet.len < 126) {
    groov_ws_packet__switch_state(PAYLOAD);
  } else if (packet.len == 126) {
    extended_bytes_count = 2;
    packet.len = 0;
    groov_ws_packet__switch_state(EXTENDED_PAYLOAD_LEN);
  } else if (packet.len == 127) {
    extended_bytes_count = 4;
    packet.len = 0;
    groov_ws_packet__switch_state(EXTENDED_PAYLOAD_LEN);
  }
}

static void groov_ws_packet__parse_extended_payload_len(char byte) {
  uint64_t increment = ((uint64_t) byte) << (8 * cursor);
  packet.len += increment;
  cursor++;

  if (cursor == extended_bytes_count) groov_ws_packet__switch_state(PAYLOAD);
}

static void groov_ws_packet__parse_payload(char byte) {
  packet.payload[cursor] = byte;
  cursor++;

  if (cursor == packet.len) {
    if (cursor < GROOV_MAX_PAYLOAD_LEN) packet.payload[cursor] = 0;

    groov_parse_io_message(&packet);
    groov_reset_ws_packet_parser();
  }
}

void groov_reset_ws_packet_parser() {
  groov_ws_packet__switch_state(OPCODE);
}

void groov_stream_to_ws_packet_parser(char byte) {
  switch(packet_state) {
    case OPCODE:
      groov_ws_packet__parse_opcode(byte);
      break;
    case PAYLOAD_LEN:
      groov_ws_packet__parse_payload_len(byte);
      break;
    case EXTENDED_PAYLOAD_LEN:
      groov_ws_packet__parse_extended_payload_len(byte);
      break;
    case PAYLOAD:
      groov_ws_packet__parse_payload(byte);
      break;
    default:
      break;
  }
}