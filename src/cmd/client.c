#include "loop.h"

int main() {
    io_connect_main_socket("127.0.0.1", 3000);

    while (1) io_run_loop_step();
    return 0;
}
