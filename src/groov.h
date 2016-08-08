#ifndef GROOV_GROOV_H
#define GROOV_GROOV_H

#include <uv.h>
#include <stdint.h>


/*
 * Define types and constants
 */

#define GROOV_PROTOCOL_VERSION 13

// Fixed memory

#define GROOV_MAX_MESSAGE_SIZE 65536
#define GROOV_EVENT_SIZE 131072
#define GROOV_EVENT_MAX_STACK_SIZE 100
#define GROOV_MAX_HOST_NAME_SIZE 255
#define GROOV_MAX_HANDSHAKE_SIZE 512
#define GROOV_MAX_CONTRACT_SIZE 1024
#define GROOV_MAX_EXTENDED_LEN 5
#define GROOV_MAX_PAYLOAD_LEN 131000


// Config type

typedef struct groov_config {
  uv_buf_t * host_name;
  uv_buf_t * host_address;
  int host_port;
} groov_config_t;

// Event type

typedef struct groov_event {
  int type;
  uv_buf_t * data;
} groov_event_t;

// Event stack type

typedef struct groov_event_stack {
  int len;
  groov_event_t * events;
} groov_event_stack_t;

// WS Packet type

typedef struct groov_ws_packet {
  char opcode;
  uint64_t len;
  char payload[GROOV_MAX_PAYLOAD_LEN];
} groov_ws_packet_t;

/*
 * Events types
 */


// Default event : none

#define GROOV_EVENT_TYPE_NONE 0

// Connect Event

#define GROOV_EVENT_TYPE_CONNECT 1
typedef struct groov_connect_event {
  int success;
} groov_connect_event_t;

// Handshake received Event

#define GROOV_EVENT_TYPE_HANDSHAKE 2
typedef struct groov_handshake_event {
} groov_handshake_event_t;

// IO/Connect Event

#define GROOV_EVENT_TYPE_IO_CONNECT 3
typedef struct groov_io_connect_event {
  unsigned timeout;
} groov_io_connect_event_t;

// Message Event

#define GROOV_EVENT_TYPE_MESSAGE 100
typedef struct groov_message_event {
  size_t len;
  char base[GROOV_MAX_MESSAGE_SIZE];
} groov_message_event_t;


/*
 * States
 */

#define GROOV_STATE_BOOT 0
#define GROOV_STATE_CONNECTED 1
#define GROOV_STATE_HANDSHAKE_RECEIVED 2


/*
 * API Declarations
 */

// Boot

void groov_init(groov_config_t *);

// Events

void groov_init_event_stack(groov_event_stack_t *, uv_buf_t *, char [GROOV_EVENT_MAX_STACK_SIZE][GROOV_EVENT_SIZE]);
int groov_write_event_to_stack(groov_event_stack_t *, int, char *, size_t);
groov_event_stack_t * groov_read_event_stack(groov_event_stack_t *, groov_event_stack_t *);
int groov_read_connect_event(groov_event_t *, groov_connect_event_t *);
int groov_read_handshake_event(groov_event_t *, groov_handshake_event_t *);

// Handshake parser

void groov_reset_handshake_parser();
void groov_stream_to_handshake_parser(char);

// Incoming Events

void groov_init_incoming_events(groov_config_t *);
groov_event_stack_t * groov_read_incoming_events();
void groov_write_incoming_connect_event(int);
void groov_write_incoming_handshake_event();

// Outgoing Events

void groov_init_outgoing_events(groov_config_t *);
void groov_write_outgoing_handshake_request();
groov_event_stack_t * groov_read_outgoing_events();

// Loop

void groov_init_loop(groov_config_t *);
void groov_run_loop_step();

// IO

void groov_parse_io_message(groov_ws_packet_t *);

// Packet

void groov_reset_ws_packet_parser();
void groov_stream_to_ws_packet_parser(char);

// Serializer

void groov_init_serialize(groov_config_t *);
void groov_serialize_handshake_request(groov_message_event_t *);

// State

void groov_init_state(groov_config_t *);
void groov_update_state(groov_event_t *);
void groov_stream_by_state(char *, ssize_t);

#endif