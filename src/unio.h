#ifndef UNIO_UNIO_H
#define UNIO_UNIO_H

#include <uv.h>


/*
 * Constants
 */

#define UNIO_READ_BUF_SIZE = 2048;


/*
 * Types Declarations
 */

typedef uv_buf_t unio_text_t;

typedef struct unio_config {
  unio_text_t * host_address;
  int host_port;
} unio_config_t;

typedef struct unio_event {
  unio_text_t * name;
  unio_text_t * data;
} unio_event_t;

typedef struct unio_event_stack {
  int len;
  unio_event_t * events;
} unio_event_stack_t;


/*
 * API Declarations
 */

void unio_init(unio_config_t *);

// Events

void unio_init_events(unio_config_t *);
void unio_read_events(unio_event_stack_t *);
void unio_write_event(unio_event_t *);

// Loop

void unio_init_loop(unio_config_t *);
void unio_run_loop_step();

#endif