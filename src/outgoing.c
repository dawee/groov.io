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

/*
 * Message construction buffer
 */

groov_message_event_t message_buf;


void groov_init_outgoing_events(groov_config_t * config) {
  groov_init_event_stack(&stack, stack_bufs, stack_memory);
  groov_init_event_stack(&stack_copy, stack_copy_bufs, stack_copy_memory);
}

groov_event_stack_t * groov_read_outgoing_events() {
  return groov_read_event_stack(&stack_copy, &stack);
}

void groov_write_outgoing_handshake_request() {
  groov_serialize_handshake_request(&message_buf);
  groov_write_outgoing_message(&message_buf);
}

void groov_write_outgoing_message(groov_message_event_t * message) {
  groov_write_event_to_stack(&stack, GROOV_EVENT_TYPE_MESSAGE, message->base, message->len);
}
