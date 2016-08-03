#include "unio.h"

#include <stdio.h>

static uv_loop_t loop;
static uv_tcp_t client;
static uv_connect_t connection;
static struct sockaddr_in address;


static void unio_loop__on_connect(uv_connect_t * new_connection, int status) {
  unio_write_incoming_connect_event(status == 0);
}

void unio_init_loop(unio_config_t * config) {
  // Make sure host address is readable as 'C String'
  config->host_address->base[config->host_address->len] = 0;

  uv_loop_init(&loop);
  uv_tcp_init(&loop, &client);
  uv_ip4_addr(config->host_address->base, config->host_port, &address);
  uv_tcp_connect(&connection, &client, (const struct sockaddr*)&address, unio_loop__on_connect);
}

void unio_run_loop_step() {
  uv_run(&loop, UV_RUN_ONCE);
}
