#ifndef UNIO_UNIO_H
#define UNIO_UNIO_H

#include <uv.h>


/*
 * Define types and constants
 */

#define UNIO_PROTOCOL_VERSION 13

// Fixed memory

#define UNIO_MAX_MESSAGE_SIZE 65536
#define UNIO_EVENT_SIZE 131072
#define UNIO_EVENT_STACK_SIZE 100
#define UNIO_READ_BUF_SIZE 2048
#define UNIO_MAX_HOST_NAME_SIZE 255

// Config type

typedef struct unio_config {
  uv_buf_t * host_name;
  uv_buf_t * host_address;
  int host_port;
} unio_config_t;

// Event type

typedef struct unio_event {
  int type;
  uv_buf_t * data;
} unio_event_t;

// Event stack type

typedef struct unio_event_stack {
  int len;
  unio_event_t * events;
} unio_event_stack_t;

/*
 * Events types
 */


// Default event : none

#define UNIO_EVENT_TYPE_NONE 0

// Connect Event

#define UNIO_EVENT_TYPE_CONNECT 1
typedef struct unio_connect_event {
  int success;
} unio_connect_event_t;

// Message Event

#define UNIO_EVENT_TYPE_MESSAGE 100
typedef struct unio_message_event {
  size_t len;
  char base[UNIO_MAX_MESSAGE_SIZE];
} unio_message_event_t;


/*
 * States
 */

#define UNIO_STATE_BOOT 0
#define UNIO_STATE_CONNECTED 1


/*
 * API Declarations
 */

// Boot

void unio_init(unio_config_t *);

// Events

void unio_init_event_stack(unio_event_stack_t *, uv_buf_t *, char [UNIO_EVENT_STACK_SIZE][UNIO_EVENT_SIZE]);
int unio_read_connect_event(unio_event_t *, unio_connect_event_t *);
int unio_write_event_to_stack(unio_event_stack_t *, int, char *, size_t);
unio_event_stack_t * unio_read_event_stack(unio_event_stack_t *, unio_event_stack_t *);

// Incoming Events

void unio_init_incoming_events(unio_config_t *);
unio_event_stack_t * unio_read_incoming_events();
void unio_write_incoming_connect_event(int);

// Outgoing Events

void unio_init_outgoing_events(unio_config_t *);
void unio_write_outgoing_handshake_request();
unio_event_stack_t * unio_read_outgoing_events();

// Loop

void unio_init_loop(unio_config_t *);
void unio_run_loop_step();

// Serializer

void unio_init_serialize(unio_config_t *);
void unio_serialize_handshake_request(unio_message_event_t *);

// State

void unio_init_state(unio_config_t *);
void unio_update_state(unio_event_t *);

#endif