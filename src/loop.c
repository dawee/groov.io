#include "groov.h"

#include <stdio.h>

static uv_loop_t loop;
static uv_tcp_t client;
static uv_connect_t connection;
static struct sockaddr_in address;
static int write_requests_count = 0;
static uv_write_t write_requests[GROOV_EVENT_MAX_STACK_SIZE];
static int connected = 0;

static void groov_loop__on_connect(uv_connect_t * new_connection, int status) {
  if (status == 0) connected = 1;

  groov_write_incoming_connect_event(status == 0);
}

static void on_write(uv_write_t* req, int status) {
  write_requests_count--;
}

static void groov_loop__write_requests() {
  int index = 0;
  groov_event_stack_t * event_stack = groov_read_outgoing_events();

  for (index = 0; index < event_stack->len; ++index) {
    uv_write(&(write_requests[index]), connection.handle, event_stack->events[index].data, 1, on_write);
  }

  write_requests_count = event_stack->len;
}

void groov_init_loop(groov_config_t * config) {
  // Make sure host address is readable as 'C String'
  config->host_address->base[config->host_address->len] = 0;

  uv_loop_init(&loop);
  uv_tcp_init(&loop, &client);
  uv_ip4_addr(config->host_address->base, config->host_port, &address);
  uv_tcp_connect(&connection, &client, (const struct sockaddr*)&address, groov_loop__on_connect);
}

void groov_run_loop_step() {
  if (connected == 1 && write_requests_count == 0) groov_loop__write_requests();
  uv_run(&loop, UV_RUN_ONCE);
}
