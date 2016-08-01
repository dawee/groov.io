#ifndef UNIO_SOCKET_LOOP_H
#define UNIO_SOCKET_LOOP_H

#include "unio.h"

/*
 * Constants
 */

#define UNIO_READ_BUF_SIZE = 2048;


/*
 * API Declarations
 */

void unio_init_socket_loop(unio_config_t *);

#endif