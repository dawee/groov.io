#include "groov.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

static char log_message[GROOV_MAX_BLOB_SIZE];

void groov_init_logger(groov_config_t * config) {
}

void groov_log(char * filename, int line, char * fmt,...) {
  va_list list;

  va_start(list, fmt);
  sprintf(log_message, fmt, list);
  groov_write_incoming_logging_event(0, log_message);
  va_end( list );
}

