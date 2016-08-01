#include <stdio.h>

#include "socket_loop.h"


static uv_loop_t loop;
static uv_tcp_t client;
static uv_connect_t connection;
static struct sockaddr_in address;


static void on_connect(uv_connect_t * new_connection, int status) {}


void unio_init_socket_loop(unio_config_t * config) {
  // Make sure host address is readable as 'C String'
  config->host_address->base[config->host_address->len] = 0;

  uv_loop_init(&loop);
  uv_tcp_init(&loop, &client);
  uv_ip4_addr(config->host_address->base, config->host_port, &address);
  uv_tcp_connect(&connection, &client, (const struct sockaddr*)&address, on_connect);
}


void unio_run_socket_loop_step() {
  uv_run(&loop, UV_RUN_ONCE);
}
