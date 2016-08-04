#include "groov.h"


static int received = 0;
static char buf_base[GROOV_MAX_CONTRACT_SIZE];
static uv_buf_t buf = {.base = buf_base};


void groov_stream_to_contract_parser(char byte) {
  if (received || buf.len == GROOV_MAX_CONTRACT_SIZE) return;

  buf.base[buf.len] = byte;
  buf.len++;
}
