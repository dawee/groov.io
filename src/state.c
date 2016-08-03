#include "unio.h"

static int state;

void unio_init_state(unio_config_t * config) {
  state = UNIO_STATE_BOOT;
}

void unio_update_state(unio_event_t * event) {
  unio_connect_event_t connect_event;

  if (state == UNIO_STATE_BOOT) {
    state = UNIO_STATE_CONNECTED;
    unio_write_outgoing_handshake_request();
  }
}

int unio_read_state() {
  return state;
}