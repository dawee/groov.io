#include <uv.h>
#include <stdio.h>


uv_tcp_t server;
uv_tcp_t client;
uv_loop_t *loop;
uv_connect_t example_connect;
struct sockaddr_in dest;

char read_buffer[65536];
char print_buffer[65536];
char write_buffer[65536];

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    *buf = uv_buf_init(read_buffer, 65536);
}

void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    printf("[%s]\n", buf->base);
}


void on_new_connection(uv_stream_t *server, int status) {
    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        // error!
        return;
    }

    uv_tcp_init(loop, &client);
    if (uv_accept(server, (uv_stream_t*) &client) == 0) {
        uv_read_start((uv_stream_t*) &client, alloc_buffer, on_read);
    }
    else {
        uv_close((uv_handle_t*) &client, NULL);
    }
}

int main() {
    loop = uv_loop_new();

    uv_tcp_init(loop, &server);
    uv_ip4_addr("0.0.0.0", 3000, &dest);

    uv_tcp_bind(&server, (const struct sockaddr*)&dest, 0);
    uv_listen((uv_stream_t*) &server, 10, on_new_connection);

    return uv_run(loop, UV_RUN_DEFAULT);
}
