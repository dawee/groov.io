#include "groov.h"

#include <stdio.h>

static groov_config_t groov_config;
static char port_str[8];
static uv_loop_t loop;
static uv_getaddrinfo_t getaddrinfo_req;
static uv_tcp_t client;
static uv_connect_t connection;
static struct sockaddr_in address;
static int write_requests_count = 0;
static uv_write_t write_requests[GROOV_EVENT_MAX_STACK_SIZE];
static int connected = 0;
static char read_buf_base[GROOV_MAX_MESSAGE_SIZE];
static uv_buf_t read_buf = {.base = read_buf_base};

static void groov_loop__alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
  read_buf.len = GROOV_MAX_MESSAGE_SIZE;
  *buf = read_buf;
}

static void groov_loop__on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
  groov_stream_by_state(buf->base, nread);
}

static void groov_loop__on_connect(uv_connect_t * new_connection, int status) {
  groov_write_incoming_connect_event(status == 0);

  if (status == 0) {
    connected = 1;
    uv_read_start(connection.handle, groov_loop__alloc_buffer, groov_loop__on_read);
  }
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

static void on_getaddrinfo(uv_getaddrinfo_t * req, int status, struct addrinfo * res) {
  address = *((struct sockaddr_in *)res->ai_addr);

  uv_tcp_connect(&connection, &client, (const struct sockaddr*)&address, groov_loop__on_connect);
  uv_freeaddrinfo(res);
}

void groov_connect() {
  sprintf(port_str, "%d", groov_config.host_port);
  uv_getaddrinfo(&loop, &getaddrinfo_req, on_getaddrinfo, groov_config.host_name, port_str, NULL);
}

void groov_init_loop(groov_config_t * config) {
  groov_config = *config;

  uv_loop_init(&loop);
  uv_tcp_init(&loop, &client);
}

void groov_run_loop_step() {
  if (connected == 1 && write_requests_count == 0) groov_loop__write_requests();
  uv_run(&loop, UV_RUN_NOWAIT);
}
