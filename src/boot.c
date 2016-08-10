#include "groov.h"
#include <time.h>
#include <stdlib.h>

void groov_init(groov_config_t * config) {
  srand(time(NULL));

  groov_init_loop(config);
  groov_init_logger(config);
  groov_init_incoming_events(config);
  groov_init_outgoing_events(config);
  groov_init_handshake(config);
  groov_init_state(config);
}
