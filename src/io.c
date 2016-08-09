#include "groov.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <slre.h>

static char const IO_PACKET_TYPE_CONNECT = '0';
static char const IO_PACKET_TYPE_CLOSE = '1';
static char const IO_PACKET_TYPE_PING = '2';
static char const IO_PACKET_TYPE_PONG = '3';
static char const IO_PACKET_TYPE_MESSAGE = '4';

static const char * ping_name = "groov";

static groov_ws_packet_header_t packet_header;
// static groov_ws_packet_ext16_header_t packet_header_ext16;
// static groov_ws_packet_ext64_header_t packet_header_ext64;

static groov_message_event_t outgoing;

static void groov_io__serialize_message(char cmd, char * message, uint64_t content_len) {
  char fin = 0x80;
  char opcode = 0x01;
  char mask = 0x80;
  char current_mask;
  uint64_t index = 0;
  uint64_t len = content_len + 1;
  uint64_t header_size = sizeof(packet_header);

  packet_header.reserved_and_opcode = fin + opcode;
  packet_header.mask_and_len = ((char) len) | mask;
  packet_header.masking_key[0] = 0x88;
  packet_header.masking_key[1] = 0x77;
  packet_header.masking_key[2] = 0x66;
  packet_header.masking_key[3] = 0x55;

  memcpy(outgoing.base, (char *) &packet_header, header_size);
  outgoing.len = header_size + len;

  for (index = 0; index < len; ++index) {
    current_mask = packet_header.masking_key[(index % 4)];
    
    if (index == 0) {
      outgoing.base[header_size + index] = cmd ^ current_mask;
    } else {
      outgoing.base[header_size + index] = message[index - 1] ^ current_mask;
    }
  }

  groov_write_outgoing_message(&outgoing);
}

static void groov_io__parse_connect(char * message, size_t len) {
  const char * connect = "^{.*\"pingTimeout\":([0-9]+).*}$";
  char timeout_str[16];
  struct slre_cap caps[1];

  if (slre_match(connect, message, len, caps, 1, 0) >= 0) {
    sprintf(timeout_str, "%.*s", caps[0].len, caps[0].ptr);
    groov_write_incoming_io_open_event(atoi(timeout_str));
  }
}

void groov_send_io_ping() {
  groov_io__serialize_message(IO_PACKET_TYPE_PING, (char *) ping_name, strlen(ping_name));
}

void groov_parse_io_message(groov_ws_packet_t * packet) {
  char io_cmd = packet->payload[0];

  if (packet->opcode != 1 || packet->len < 1) return;

  if (io_cmd == IO_PACKET_TYPE_CONNECT) {
    groov_io__parse_connect(&(packet->payload[1]), packet->len - 1);
  } else if (io_cmd == IO_PACKET_TYPE_PONG) {
    GROOV_DEBUG_LOG("Server sent pong !");
  }
}