#include <uv.h>
#include <stdio.h>

uv_tcp_t example_socket;
uv_loop_t *loop;
uv_connect_t example_connect;
struct sockaddr_in dest;

void on_connect(uv_connect_t *req) {

}


int main() {
    loop = uv_loop_new();

    uv_tcp_init(loop, &example_socket);
    uv_ip4_addr("127.0.0.1", 80, &dest);

    uv_tcp_connect(&example_connect, &example_socket, (const struct sockaddr*)&dest, on_connect);

    return uv_run(loop, UV_RUN_DEFAULT);
}