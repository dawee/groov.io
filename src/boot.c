#include "unio.h"


void unio_init(unio_config_t * config) {
  unio_init_loop(config);
  unio_init_incoming_events(config);
  unio_init_outgoing_events(config);
  unio_init_serialize(config);
  unio_init_state(config);
}
