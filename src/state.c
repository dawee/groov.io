#include "groov.h"

static int state;

void groov_init_state(groov_config_t * config) {
  state = GROOV_STATE_BOOT;
}

void groov_update_state(groov_event_t * event) {
  groov_connect_event_t connect_event;
  groov_handshake_event_t handshake_event;
  groov_io_open_event_t io_connect_event;

  if (state == GROOV_STATE_BOOT && groov_read_connect_event(event, &connect_event) && connect_event.success) {
    state = GROOV_STATE_CONNECTED;
    groov_write_outgoing_handshake_request();
    groov_reset_handshake_parser();
  } else if (state == GROOV_STATE_CONNECTED && groov_read_handshake_event(event, &handshake_event)) {
    state = GROOV_STATE_HANDSHAKE_RECEIVED;
    groov_reset_ws_packet_parser();
  } else if (state == GROOV_STATE_HANDSHAKE_RECEIVED && groov_read_io_open_event(event, &io_connect_event)) {
    state = GROOV_STATE_IO_OPEN;
    groov_send_io_ping();
  }
}

void groov_stream_by_state(char * data, ssize_t size) {
  unsigned index = 0;
  char byte = 0;

  for (index = 0; index < size; ++index) {
    byte = data[index];

    switch (state) {
      case GROOV_STATE_CONNECTED:
        groov_stream_to_handshake_parser(byte);
        break;
      case GROOV_STATE_HANDSHAKE_RECEIVED:
      case GROOV_STATE_IO_OPEN:
        groov_stream_to_ws_packet_parser(byte);
        break;
      default:
        break;
    };
  }
}
