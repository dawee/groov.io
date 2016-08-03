#include "groov.h"


void groov_init(groov_config_t * config) {
  groov_init_loop(config);
  groov_init_incoming_events(config);
  groov_init_outgoing_events(config);
  groov_init_serialize(config);
  groov_init_state(config);
}
