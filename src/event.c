#include "unio.h"

#include <string.h>


void unio_init_event_stack(unio_event_stack_t * stack, uv_buf_t * stack_bufs, char stack_memory[UNIO_EVENT_STACK_SIZE][UNIO_EVENT_SIZE]) {
  int index = 0;

  stack->len = 0;

  for (index = 0; index < UNIO_EVENT_STACK_SIZE; ++index) {
    stack->events[index].type = UNIO_EVENT_TYPE_NONE;
    stack->events[index].data = &(stack_bufs[index]);
    stack->events[index].data->len = UNIO_EVENT_SIZE;
    stack->events[index].data->base = stack_memory[index];
  }
}

int unio_write_event_to_stack(unio_event_stack_t * stack, int type, char * data, size_t len) {
  if (stack->len >= UNIO_EVENT_STACK_SIZE) return 0;

  stack->events[stack->len].type = type;
  stack->events[stack->len].data->len = len;
  memcpy(stack->events[stack->len].data->base, data, len);

  unio_update_state(&(stack->events[stack->len]));

  stack->len++;

  return 1;
}

unio_event_stack_t * unio_read_event_stack(unio_event_stack_t * stack_copy, unio_event_stack_t * stack) {
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

int unio_read_connect_event(unio_event_t * event, unio_connect_event_t * connect_event) {
  if (event->type != UNIO_EVENT_TYPE_CONNECT) return 0;

  memcpy(connect_event, event->data->base, event->data->len);
  return 1;
}
