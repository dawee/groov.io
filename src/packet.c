#include "groov.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum {
  OPCODE,
  PAYLOAD_LEN,
  EXTENDED_PAYLOAD_LEN,
  PAYLOAD,
} groov_ws_packet__state_t;

static unsigned WS_EXT_LEN_16_LIMIT = 65535;
static unsigned WS_EXT_LEN_7_LIMIT = 125;
static const char WS_OPCODE_TEXT = 0x01;
static const char MASK_ON = 0x80;

static groov_ws_packet_t packet;
static groov_ws_packet__state_t packet_state;
static unsigned long cursor;
static unsigned extended_bytes_count;
static groov_message_event_t outgoing;
static const char * masking_key_charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

static groov_ws_packet_header_t packet_header;
static groov_ws_packet_ext16_header_t packet_header_ext16;
static groov_ws_packet_ext64_header_t packet_header_ext64;

static void groov_ws_packet__switch_state(groov_ws_packet__state_t new_state) {
  packet_state = new_state;
  cursor = 0;
}

static void groov_ws_packet__parse_opcode(char byte) {
  packet.opcode = byte & 0x0F;
  groov_ws_packet__switch_state(PAYLOAD_LEN);
}

static void groov_ws_packet__parse_payload_len(char byte) {
  packet.len = byte & 0x7F;

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

static void groov_ws_packet__feed_masking_key(char masking_key[4]) {
  size_t index = 0;
  int key = 0;

  for (index = 0; index < 4; ++index) {
    key = rand() % (int) (sizeof(masking_key_charset) - 1);
    masking_key[index] = masking_key_charset[key];
  }
}

static void groov_ws_packet__feed_reserved_and_opcode(unsigned char * reserved_and_opcode, char opcode) {
  char fin = 0x80;
  *reserved_and_opcode = fin + opcode;
}

static void groov_ws_packet__feed_mask_and_len(unsigned char * mask_and_len, char mask, uint64_t len) {
  if (len > WS_EXT_LEN_16_LIMIT) {
    *mask_and_len = 127 | mask;
  } else if (len > WS_EXT_LEN_7_LIMIT) {
    *mask_and_len = 126 | mask;
  } else {
    *mask_and_len = ((char) len) | mask;
  }
}

static void groov_ws_packet__feed_outgoing(char * header, uint64_t header_size, char * payload, uint64_t len, char * masking_key) {
  char current_mask;
  uint64_t index = 0;

  memcpy(outgoing.base, header, header_size);

  for (index = 0; index < len; ++index) {
    current_mask = masking_key[(index % 4)];
    outgoing.base[header_size + index] = payload[index] ^ current_mask;
  }

  outgoing.len = header_size + len;
}

static void groov_ws_packet__serialize_packet(char * payload, uint64_t len) {
  uint64_t header_size = sizeof(packet_header);

  groov_ws_packet__feed_reserved_and_opcode(&(packet_header.reserved_and_opcode), WS_OPCODE_TEXT);
  groov_ws_packet__feed_mask_and_len(&(packet_header.mask_and_len), MASK_ON, len);  
  groov_ws_packet__feed_masking_key(packet_header.masking_key);
  groov_ws_packet__feed_outgoing((char *) &packet_header, header_size, payload, len, packet_header.masking_key);
}

static void groov_ws_packet__serialize_ext16_packet(char * payload, uint64_t len) {
  uint64_t header_size = sizeof(packet_header_ext16);

  packet_header_ext16.extended_len = len;

  groov_ws_packet__feed_reserved_and_opcode(&(packet_header_ext16.reserved_and_opcode), WS_OPCODE_TEXT);
  groov_ws_packet__feed_mask_and_len(&(packet_header_ext16.mask_and_len), MASK_ON, len);  
  groov_ws_packet__feed_masking_key(packet_header_ext16.masking_key);
  groov_ws_packet__feed_outgoing((char *) &packet_header_ext16, header_size, payload, len, packet_header_ext16.masking_key);
}

static void groov_ws_packet__serialize_ext64_packet(char * payload, uint64_t len) {
  uint64_t header_size = sizeof(packet_header_ext64);

  packet_header_ext64.extended_len = len;

  groov_ws_packet__feed_reserved_and_opcode(&(packet_header_ext64.reserved_and_opcode), WS_OPCODE_TEXT);
  groov_ws_packet__feed_mask_and_len(&(packet_header_ext64.mask_and_len), MASK_ON, len);  
  groov_ws_packet__feed_masking_key(packet_header_ext64.masking_key);
  groov_ws_packet__feed_outgoing((char *) &packet_header_ext64, header_size, payload, len, packet_header_ext64.masking_key);
}


void groov_send_ws_message(char * payload, uint64_t len) {
  if (len > WS_EXT_LEN_16_LIMIT) {
    groov_ws_packet__serialize_ext64_packet(payload, len);
  } else if (len > WS_EXT_LEN_7_LIMIT) {
    groov_ws_packet__serialize_ext16_packet(payload, len);
  } else {
    groov_ws_packet__serialize_packet(payload, len);
  }

  groov_write_outgoing_message(&outgoing);
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