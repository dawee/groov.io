#include "unio.h"
#include "socket_loop.h"

#include <stdio.h>


void unio_init(unio_config_t * config) {
  unio_init_socket_loop(config);
}


void unio_read_events(unio_event_stack_t * events) {
  unio_run_socket_loop_step();
}


void unio_write_event(unio_event_t * event) {
}
