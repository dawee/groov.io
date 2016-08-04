#include "groov.h"

#include <string.h>


static uv_buf_t event_buf;


void groov_init_event_stack(groov_event_stack_t * stack, uv_buf_t * stack_bufs, char stack_memory[GROOV_EVENT_MAX_STACK_SIZE][GROOV_EVENT_SIZE]) {
  int index = 0;

  stack->len = 0;

  for (index = 0; index < GROOV_EVENT_MAX_STACK_SIZE; ++index) {
    stack->events[index].type = GROOV_EVENT_TYPE_NONE;
    stack->events[index].data = &(stack_bufs[index]);
    stack->events[index].data->len = GROOV_EVENT_SIZE;
    stack->events[index].data->base = stack_memory[index];
  }
}

int groov_write_event_to_stack(groov_event_stack_t * stack, int type, char * data, size_t len) {
  if (stack->len >= GROOV_EVENT_MAX_STACK_SIZE) return 0;

  stack->events[stack->len].type = type;
  stack->events[stack->len].data->len = len;
  memcpy(stack->events[stack->len].data->base, data, len);

  groov_update_state(&(stack->events[stack->len]));

  stack->len++;

  return 1;
}

groov_event_stack_t * groov_read_event_stack(groov_event_stack_t * stack_copy, groov_event_stack_t * stack) {
  int index = 0;  

  stack_copy->len = stack->len;

  for (index = 0; index < stack->len; ++index) {
    // Copy len values
    stack_copy->events[index].type = stack->events[index].type;
    stack_copy->events[index].data->len = stack->events[index].data->len;

    // Copy Buffers content
    memcpy(
      stack_copy->events[index].data->base,
      stack->events[index].data->base,
      stack->events[index].data->len
    );
  }

  stack->len = 0;

  return stack_copy;
}

int groov_read_connect_event(groov_event_t * event, groov_connect_event_t * connect_event) {
  if (event->type != GROOV_EVENT_TYPE_CONNECT) return 0;

  memcpy(connect_event, event->data->base, event->data->len);
  return 1;
}

int groov_read_handshake_event(groov_event_t * event, groov_handshake_event_t * handshake_event) {
  if (event->type != GROOV_EVENT_TYPE_HANDSHAKE) return 0;

  memcpy(handshake_event, event->data->base, event->data->len);
  return 1;
}
