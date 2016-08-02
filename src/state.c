#include "unio.h"

static int state;

void unio_init_state_machine(unio_config_t * config) {
  state = UNIO_STATE_BOOT;
}

void unio_update_state_machine(unio_event_t * event) {
  if (state == UNIO_STATE_BOOT && event->type == UNIO_EVENT_TYPE_CONNECT) {
  }
}