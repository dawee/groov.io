#include "unio.h"


void unio_init_events(unio_config_t * config) {}


void unio_read_events(unio_event_stack_t * events) {
  unio_run_loop_step();
}


void unio_write_event(unio_event_t * event) {
}