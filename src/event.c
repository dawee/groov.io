#include "unio.h"


static char stack_memory[UNIO_EVENT_STACK_SIZE][UNIO_EVENT_SIZE];
static uv_buf_t stack_data[UNIO_EVENT_STACK_SIZE];
static unio_event_t stack_events[UNIO_EVENT_STACK_SIZE];
static unio_event_stack_t stack = {.len = UNIO_EVENT_STACK_SIZE, .events = stack_events};


void unio_init_events(unio_config_t * config) {
  int index = 0;

  for (index = 0; index < UNIO_EVENT_STACK_SIZE; ++index) {
    stack.events[index].type = UNIO_EVENT_TYPE_NONE;
    stack.events[index].data = &(stack_data[index]);
    stack.events[index].data->len = UNIO_EVENT_SIZE;
    stack.events[index].data->base = stack_memory[index];
  }
}

int unio_read_events(unio_event_stack_t * events) {
  unio_run_loop_step();
}


void unio_write_event(unio_event_t * event) {

}