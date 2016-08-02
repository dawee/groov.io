#include "unio.h"

#include <string.h>

/*
 * Events Stack
 */

static char stack_memory[UNIO_EVENT_STACK_SIZE][UNIO_EVENT_SIZE];
static uv_buf_t stack_data[UNIO_EVENT_STACK_SIZE];
static unio_event_t stack_events[UNIO_EVENT_STACK_SIZE];
static unio_event_stack_t stack = {.len = 0, .events = stack_events};

/*
 * Read copy of the event stack
 */

static char stack_copy_memory[UNIO_EVENT_STACK_SIZE][UNIO_EVENT_SIZE];
static uv_buf_t stack_copy_data[UNIO_EVENT_STACK_SIZE];
static unio_event_t stack_copy_events[UNIO_EVENT_STACK_SIZE];
static unio_event_stack_t stack_copy = {.len = 0, .events = stack_copy_events};


static void unio_incoming__init_stack() {
  int index = 0;

  stack.len = 0;

  for (index = 0; index < UNIO_EVENT_STACK_SIZE; ++index) {
    stack.events[index].type = UNIO_EVENT_TYPE_NONE;
    stack.events[index].data = &(stack_data[index]);
    stack.events[index].data->len = UNIO_EVENT_SIZE;
    stack.events[index].data->base = stack_memory[index];
  }
}

static void unio_incoming__copy_stack() {
  int index = 0;

  stack_copy.len = stack.len;

  for (index = 0; index < stack.len; ++index) {
    // Link stack to its memory
    stack_copy.events[index].data = &(stack_copy_data[index]);
    stack_copy.events[index].data->len = UNIO_EVENT_SIZE;
    stack_copy.events[index].data->base = stack_copy_memory[index];

    // Copy len values
    stack_copy.events[index].type = stack.events[index].type;
    stack_copy.events[index].data->len = stack.events[index].data->len;
    
    // Copy Buffers content
    memcpy(
      stack_copy.events[index].data->base,
      stack.events[index].data->base,
      stack.events[index].data->len
    );
  }
}

void unio_init_incoming_events(unio_config_t * config) {
  unio_incoming__init_stack();
  unio_incoming__copy_stack();
}

unio_event_stack_t * unio_read_incoming_events() {
  unio_run_loop_step();
  unio_incoming__copy_stack();
  
  stack.len = 0;

  return &stack_copy;
}

void unio_write_connect_event(int success) {
  unio_connect_event_t event = {.success = success};
  unio_write_event_to_stack(&stack, UNIO_EVENT_TYPE_CONNECT, (char *) &event, sizeof(event));
}