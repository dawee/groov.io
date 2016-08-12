#include "groov.h"

#include <stdio.h>

static char sending = 0;

static groov_config_t groov_config;
static uv_loop_t loop;

/*
 * Writing stack
 */

static groov_event_stack_t * event_stack;
static unsigned writing_index = 0;

/*
 * TCP Socket
 */

static char port_str[8];
static uv_getaddrinfo_t getaddrinfo_req;
static uv_tcp_t client;
static uv_connect_t connection;
static struct sockaddr address;

/*
 * Buf
 */

static uv_write_t write_request;
static char read_buf_base[GROOV_MAX_BLOB_SIZE];
static uv_buf_t read_buf = {.base = read_buf_base};


static void groov_loop__alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
  read_buf.len = GROOV_MAX_BLOB_SIZE;
  *buf = read_buf;
}

static void groov_loop__on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
  groov_stream_by_state(buf->base, nread);
}

static void groov_loop__on_connect(uv_connect_t * new_connection, int status) {
  groov_write_incoming_connect_event(status == 0);
}

static void groov_loop__on_write(uv_write_t* req, int status) {
  if (status != 0) return;

  writing_index++;

  if (writing_index >= event_stack->len) {
    sending = 0;
  } else {
    uv_write(&write_request, connection.handle, event_stack->events[writing_index].data, 1, groov_loop__on_write);
  }
}

static void groov_loop__on_getaddrinfo(uv_getaddrinfo_t * req, int status, struct addrinfo * res) {
  if (status != 0) return;
  
  address = *(res->ai_addr);

  uv_tcp_connect(&connection, &client, &address, groov_loop__on_connect);
  uv_freeaddrinfo(res);
}

void groov_read_outgoing_loop_events() {
  if (sending) return;

  event_stack = groov_read_outgoing_events();
  
  if (event_stack->len == 0) return;

  sending = 1;
  writing_index = 0;
  uv_write(&write_request, connection.handle, event_stack->events[writing_index].data, 1, groov_loop__on_write);
}

void groov_start_reading_loop() {
  uv_read_start(connection.handle, groov_loop__alloc_buffer, groov_loop__on_read);
}

void groov_connect() {
  sprintf(port_str, "%d", groov_config.host_port);
  uv_getaddrinfo(&loop, &getaddrinfo_req, groov_loop__on_getaddrinfo, groov_config.host_name, port_str, NULL);
}

void groov_init_loop(groov_config_t * config) {
  groov_config = *config;

  uv_loop_init(&loop);
  uv_tcp_init(&loop, &client);
}

void groov_run_loop_step() {
  groov_prepare_step_by_state();
  uv_run(&loop, UV_RUN_NOWAIT);
}
