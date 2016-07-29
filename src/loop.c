#include <uv.h>
#include <stdio.h>

#include "loop.h"
#include "serializer.h"

static uv_tcp_t main_socket;
static uv_loop_t main_loop;
static uv_connect_t main_connection;
static struct sockaddr_in destination_address;

static char read_buffer[65536];

static uv_write_t write_req;

static char request_buf_base[65536];
static uv_buf_t handshake_request = {.base = request_buf_base};


static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    *buf = uv_buf_init(read_buffer, 65536);
}

static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    printf("%s", buf->base);
}

static void on_write(uv_write_t* req, int status) {

}

static void send_handshake_request(uv_connect_t* connection) {
    io_handshake_request_t req = {
        .hostname = "david.internal.smartimpulse.com",
        .port = 3000,
        .version = 13,
        .key = "MTMtMTQ2OTcwMDczOTcwNA=="
    };

    io_serialize_handshake_request(&handshake_request, &req);
    uv_write(&write_req, connection->handle, &handshake_request, 1, on_write);
}

void on_connect(uv_connect_t* connection, int status) {

    if (status == 0) {
        printf("connected\n");
        uv_read_start(connection->handle, alloc_buffer, on_read);
        send_handshake_request(connection);
    }
}

void io_run_loop_step() {
    uv_run(&main_loop, UV_RUN_ONCE);
}

void io_connect_main_socket(const char * ip_address, int port) {
    uv_loop_init(&main_loop);

    uv_tcp_init(&main_loop, &main_socket);
    uv_ip4_addr(ip_address, port, &destination_address);

    uv_tcp_connect(&main_connection, &main_socket, (const struct sockaddr*)&destination_address, on_connect);

}
