#include "groov.h"

static int state;

void groov_init_state(groov_config_t * config) {
  state = GROOV_STATE_BOOT;
}

void groov_update_state(groov_event_t * event) {
  groov_connect_event_t connect_event;

  if (state == GROOV_STATE_BOOT && groov_read_connect_event(event, &connect_event) && connect_event.success) {
    state = GROOV_STATE_CONNECTED;
    groov_write_outgoing_handshake_request();
  }
}

int groov_read_state() {
  return state;
}