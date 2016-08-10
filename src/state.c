#include "groov.h"


typedef enum {
  BOOT,
  CONNECTED,
  HANDSHAKE_RECEIVED,
  IO_OPEN,
} groov_ws_packet__state_t;

static groov_ws_packet__state_t state;

void groov_init_state(groov_config_t * config) {
  state = BOOT;
}

void groov_update_state(groov_event_t * event) {
  if (state == BOOT && event->type == GROOV_EVENT_TYPE_CONNECT) {
    state = CONNECTED;
    groov_write_outgoing_handshake_request();
    groov_reset_handshake_parser();
    groov_start_reading_loop();
  } else if (state == CONNECTED  && event->type == GROOV_EVENT_TYPE_HANDSHAKE) {
    state = HANDSHAKE_RECEIVED;
    groov_reset_ws_packet_parser();
  } else if (state == HANDSHAKE_RECEIVED  && event->type == GROOV_EVENT_TYPE_IO_OPEN) {
    state = IO_OPEN;
    groov_send_io_ping();
  }
}

void groov_prepare_step_by_state() {
  if (state >= CONNECTED) groov_read_outgoing_loop_events();
}

void groov_stream_by_state(char * data, ssize_t size) {
  unsigned index = 0;
  char byte = 0;

  for (index = 0; index < size; ++index) {
    byte = data[index];

    switch (state) {
      case CONNECTED:
        groov_stream_to_handshake_parser(byte);
        break;
      case HANDSHAKE_RECEIVED:
      case IO_OPEN:
        groov_stream_to_ws_packet_parser(byte);
        break;
      default:
        break;
    };
  }
}
