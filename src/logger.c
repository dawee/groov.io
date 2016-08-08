#include "groov.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

static FILE * log_file;

void groov_init_logger(groov_config_t * config) {
  log_file = fopen ("groov.log", "a");
}

void groov_log(char * filename, int line, char * fmt,...) {
  va_list list;

  va_start(list, fmt);
  vfprintf(log_file, fmt, list);
  fflush(log_file);
  fputc( '\n', log_file );
  fflush(log_file);
  va_end( list );
}

