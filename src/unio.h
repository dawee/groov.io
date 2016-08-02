#ifndef UNIO_UNIO_H
#define UNIO_UNIO_H

#include <uv.h>


/*
 * Constants
 */

#define UNIO_EVENT_SIZE 65536
#define UNIO_EVENT_STACK_SIZE 100
#define UNIO_READ_BUF_SIZE 2048

/*
 * Event Type Constants
 */

#define UNIO_EVENT_TYPE_NONE 0

/*
 * Types Declarations
 */

typedef struct unio_config {
  uv_buf_t * host_address;
  int host_port;
} unio_config_t;

typedef struct unio_event {
  int type;
  uv_buf_t * data;
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
int unio_read_events(unio_event_stack_t *);
void unio_write_event(unio_event_t *);

// Loop

void unio_init_loop(unio_config_t *);
void unio_run_loop_step();

#endif