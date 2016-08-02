#include "unio.h"

#include <string.h>


static char stack_memory[UNIO_EVENT_STACK_SIZE][UNIO_EVENT_SIZE];
static uv_buf_t stack_data[UNIO_EVENT_STACK_SIZE];
static unio_event_t stack_events[UNIO_EVENT_STACK_SIZE];
static unio_event_stack_t stack = {.len = 0, .events = stack_events};


static void unio__write_event(int type, char * data, size_t len) {
  if (stack.len >= UNIO_EVENT_STACK_SIZE) return;

  stack.events[stack.len].type = type;
  stack.events[stack.len].data->len = len;
  memcpy(stack.events[stack.len].data->base, data, len);
  stack.len++;
}

static void unio__reset_stack() {
  int index = 0;

  for (index = 0; index < UNIO_EVENT_STACK_SIZE; ++index) {
    stack.events[index].type = UNIO_EVENT_TYPE_NONE;
    stack.events[index].data = &(stack_data[index]);
    stack.events[index].data->len = UNIO_EVENT_SIZE;
    stack.events[index].data->base = stack_memory[index];
  }
}

void unio_init_incoming_events(unio_config_t * config) {
  unio__reset_stack();
}

int unio_read_incoming_events() {
  int count = 0;

  unio_run_loop_step();
  count = stack.len;
  unio__reset_stack();

  return count;
}

void unio_write_connect_event(int success) {
  unio_connect_event_t event = {.success = success};
  unio__write_event(UNIO_EVENT_TYPE_CONNECT, (char *) &event, sizeof(event));
}