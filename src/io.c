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
static char outgoing_message[GROOV_MAX_BLOB_SIZE];

static void groov_io__parse_connect(char * message, size_t len) {
  const char * connect = "^{.*\"pingTimeout\":([0-9]+).*}$";
  char timeout_str[16];
  struct slre_cap caps[1];

  if (slre_match(connect, message, len, caps, 1, 0) >= 0) {
    sprintf(timeout_str, "%.*s", caps[0].len, caps[0].ptr);
    groov_write_incoming_io_open_event(atoi(timeout_str));
  }
}

static void groov_io__send_message(char cmd, char * message, uint64_t len) {
  outgoing_message[0] = cmd;
  memcpy(&(outgoing_message[1]), message, len);
  groov_send_ws_packet(outgoing_message, len + 1);
}

void groov_send_io_ping() {
  groov_io__send_message(IO_PACKET_TYPE_PING, (char *) ping_name, 5);
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