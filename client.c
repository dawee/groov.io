#include <uv.h>
#include <stdio.h>

uv_tcp_t example_socket;
uv_loop_t loop;
uv_connect_t example_connect;
struct sockaddr_in dest;

char read_buffer[65536];
char write_buffer[65536];

uv_write_t write_req;

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    *buf = uv_buf_init(read_buffer, 65536);
}

void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    printf("%s", buf->base);
}

void on_write(uv_write_t* req, int status) {

}

void send_handshake_request(uv_connect_t* connection) {
    uv_buf_t handshake_request[] = {
        { .base = "GET /socket.io/?EIO=3&transport=websocket HTTP/1.1\nConnection: Upgrade\nUpgrade: websocket\nHost: david.internal.smartimpulse.com:3000\nSec-WebSocket-Version: 13\nSec-WebSocket-Key: MTMtMTQ2OTcwMDczOTcwNA==\nSec-WebSocket-Extensions: permessage-deflate; client_max_window_bits\n\n", .len = 273 }
    };

    uv_write(&write_req, connection->handle, handshake_request, 1, on_write);
}

void on_connect(uv_connect_t* connection, int status) {

    if (status == 0) {
        printf("connected\n");
        uv_read_start(connection->handle, alloc_buffer, on_read);
        send_handshake_request(connection);
    }
}

int main() {
    uv_loop_init(&loop);

    uv_tcp_init(&loop, &example_socket);
    uv_ip4_addr("127.0.0.1", 3000, &dest);

    uv_tcp_connect(&example_connect, &example_socket, (const struct sockaddr*)&dest, on_connect);

    return uv_run(&loop, UV_RUN_DEFAULT);
}
