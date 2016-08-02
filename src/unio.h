#ifndef UNIO_UNIO_H
#define UNIO_UNIO_H

#include <uv.h>


/*
 * Define types and constants
 */

#define UNIO_PROTOCOL_VERSION 13

// Fixed memory

#define UNIO_EVENT_SIZE 65536
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

int unio_read_connect_event(unio_event_t *, unio_connect_event_t *);
int unio_write_event_to_stack(unio_event_stack_t *, int, char *, size_t);

// Incoming Events

void unio_init_incoming_events(unio_config_t *);
unio_event_stack_t * unio_read_incoming_events();

// Outgoing Events

void unio_init_outgoing_events(unio_config_t *);

// Loop

void unio_init_loop(unio_config_t *);
void unio_run_loop_step();

// Serializer

void unio_init_serialize(unio_config_t *);

// State

void unio_init_state(unio_config_t *);
void unio_update_state(unio_event_t *);

#endif