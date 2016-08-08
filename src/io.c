#include "groov.h"

#include <string.h>
#include <slre.h>


void groov_parse_io_message(groov_ws_packet_t * packet) {
  const char * connect = ".*\"pingTimeout\":([0-9]+)\\b.*";
  struct slre_cap caps[1];

  if (slre_match(packet->payload, connect, strlen(connect), caps, 1) == 0) {
    groov_write_incoming_io_connect_event(0);
  }

}