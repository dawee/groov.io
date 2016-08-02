#include "unio.h"

#include <string.h>


int unio_read_connect_event(unio_event_t * event, unio_connect_event_t * connect_event) {
  if (event->type != UNIO_EVENT_TYPE_CONNECT) return 0;

  memcpy(connect_event, event->data->base, event->data->len);
  return 0;
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
