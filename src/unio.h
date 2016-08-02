#ifndef UNIO_UNIO_H
#define UNIO_UNIO_H

#include <uv.h>


/*
 * Define types and constants
 */


// Fixed memory

#define UNIO_EVENT_SIZE 65536
#define UNIO_EVENT_STACK_SIZE 100
#define UNIO_READ_BUF_SIZE 2048

// Config type

typedef struct unio_config {
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
 * API Declarations
 */

// Boot

void unio_init(unio_config_t *);

// Incoming Events

void unio_init_incoming_events(unio_config_t *);
unio_event_stack_t * unio_read_incoming_events();

// Loop

void unio_init_loop(unio_config_t *);
void unio_run_loop_step();

#endif