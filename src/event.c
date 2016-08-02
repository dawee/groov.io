#include "unio.h"

#include <string.h>

int unio_read_connect_event(unio_event_t * event, unio_connect_event_t * connect_event) {
  if (event->type != UNIO_EVENT_TYPE_CONNECT) return 0;

  memcpy(connect_event, event->data->base, event->data->len);
  return 0;
}
