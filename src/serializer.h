#ifndef IO_SERIALIZER_H
#define IO_SERIALIZER_H

#include <uv.h>

typedef struct io_handshake_request {
  char hostname[255];
  int port;
  int version;
  char key[32];
} io_handshake_request_t;

void io_serialize_handshake_request(uv_buf_t *, io_handshake_request_t *);

#endif