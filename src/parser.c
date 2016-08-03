#include "groov.h"

#include <string.h>

static groov_message_event_t message;

void groov_init_parser(groov_config_t * config) {
  message.len = 0;
}

void groov_feed_parser(uv_buf_t * buf) {
  if (message.len + buf->len > GROOV_MAX_MESSAGE_SIZE) return;

  memcpy(&(message.base[message.len]), buf->base, buf->len);
  message.len +=  buf->len;
}

