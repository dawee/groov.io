#include "groov.h"

#include <string.h>

/*
 * Events Stack
 */

static char stack_memory[GROOV_EVENT_MAX_STACK_SIZE][GROOV_EVENT_SIZE];
static uv_buf_t stack_bufs[GROOV_EVENT_MAX_STACK_SIZE];
static groov_event_t stack_events[GROOV_EVENT_MAX_STACK_SIZE];
static groov_event_stack_t stack = {.len = 0, .events = stack_events};

/*
 * Read copy of the event stack
 */

static char stack_copy_memory[GROOV_EVENT_MAX_STACK_SIZE][GROOV_EVENT_SIZE];
static uv_buf_t stack_copy_bufs[GROOV_EVENT_MAX_STACK_SIZE];
static groov_event_t stack_copy_events[GROOV_EVENT_MAX_STACK_SIZE];
static groov_event_stack_t stack_copy = {.len = 0, .events = stack_copy_events};


void groov_init_incoming_events(groov_config_t * config) {
  groov_init_event_stack(&stack, stack_bufs, stack_memory);
  groov_init_event_stack(&stack_copy, stack_copy_bufs, stack_copy_memory);
}

groov_event_stack_t * groov_read_incoming_events() {
  return groov_read_event_stack(&stack_copy, &stack);
}

void groov_write_incoming_connect_event(int success) {
  groov_connect_event_t event = {.success = success};
  groov_write_event_to_stack(&stack, GROOV_EVENT_TYPE_CONNECT, (char *) &event, sizeof(event));
}
