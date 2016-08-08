#include "groov.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <slre.h>

char const IO_PACKET_TYPE_CONNECT = '0';
char const IO_PACKET_TYPE_ERROR = '4';

static void groov_io__parse_connect(groov_ws_packet_t * packet) {
  const char * connect = ".*\"pingTimeout\":([0-9]+)";
  char timeout_str[16];
  struct slre_cap caps[1];

  if (slre_match(connect, packet->payload, packet->len, caps, 1, 0) >= 0) {
    sprintf(timeout_str, "%.*s", caps[0].len, caps[0].ptr);
    groov_write_incoming_io_connect_event(atoi(timeout_str));
  }
}

void groov_parse_io_message(groov_ws_packet_t * packet) {
  char io_cmd = packet->payload[0];

  if (packet->opcode != 1 || packet->len <= 0) return;

  if (io_cmd == IO_PACKET_TYPE_CONNECT) {
    groov_io__parse_connect(packet);
  }
}