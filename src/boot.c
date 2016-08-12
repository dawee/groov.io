#include "groov.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

char host_name[255];
groov_config_t groov_config;

void groov_init(groov_config_t * config) {
  sprintf(host_name, "%s", config->host_name);

  groov_config.host_port = config->host_port;
  groov_config.host_name = host_name;

  srand(time(NULL));

  groov_init_loop(&groov_config);
  groov_init_logger(&groov_config);
  groov_init_incoming_events(&groov_config);
  groov_init_outgoing_events(&groov_config);
  groov_init_handshake(&groov_config);
  groov_init_state(&groov_config);
}
