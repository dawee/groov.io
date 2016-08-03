#include "unio.h"

#include <string.h>

/*
 * Events Stack
 */

static char stack_memory[UNIO_EVENT_MAX_STACK_SIZE][UNIO_EVENT_SIZE];
static uv_buf_t stack_bufs[UNIO_EVENT_MAX_STACK_SIZE];
static unio_event_t stack_events[UNIO_EVENT_MAX_STACK_SIZE];
static unio_event_stack_t stack = {.len = 0, .events = stack_events};

/*
 * Read copy of the event stack
 */

static char stack_copy_memory[UNIO_EVENT_MAX_STACK_SIZE][UNIO_EVENT_SIZE];
static uv_buf_t stack_copy_bufs[UNIO_EVENT_MAX_STACK_SIZE];
static unio_event_t stack_copy_events[UNIO_EVENT_MAX_STACK_SIZE];
static unio_event_stack_t stack_copy = {.len = 0, .events = stack_copy_events};


void unio_init_incoming_events(unio_config_t * config) {
  unio_init_event_stack(&stack, stack_bufs, stack_memory);
  unio_init_event_stack(&stack_copy, stack_copy_bufs, stack_copy_memory);
}

unio_event_stack_t * unio_read_incoming_events() {
  return unio_read_event_stack(&stack_copy, &stack);
}

void unio_write_incoming_connect_event(int success) {
  unio_connect_event_t event = {.success = success};
  unio_write_event_to_stack(&stack, UNIO_EVENT_TYPE_CONNECT, (char *) &event, sizeof(event));
}
