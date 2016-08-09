const fs = require('fs');
const ffi = require('ffi');
const ref = require('ref');
const StructType = require('ref-struct');
const path = require('path');
const socketIO = require('socket.io');
const net = require('net');


/*
 * C Structs
 */

const uv_buf_t = StructType();
uv_buf_t.defineProperty('base', ref.types.CString);
uv_buf_t.defineProperty('len', ref.types.size_t);

const groov_config_t = StructType();
groov_config_t.defineProperty('host_name', ref.refType(uv_buf_t));
groov_config_t.defineProperty('host_address', ref.refType(uv_buf_t));
groov_config_t.defineProperty('port', ref.types.int);



const groov_event_t = StructType();
groov_event_t.defineProperty('type', ref.types.int);
groov_event_t.defineProperty('data', ref.refType(uv_buf_t));

const groov_events_t = StructType();

for (let i = 0; i < 100; ++i) {
  groov_events_t.defineProperty(`i${i}`, groov_event_t);
}


const groov_event_stack_t = StructType();
groov_event_stack_t.defineProperty('len', ref.types.int);
groov_event_stack_t.defineProperty('events', ref.refType(groov_events_t));


const GROOV_EVENT_TYPE_CONNECT = 1;
const GROOV_EVENT_TYPE_HANDSHAKE = 2;
const GROOV_EVENT_TYPE_IO_OPEN = 3;


class EventLoop {

  constructor(lib) {
    this.lib = lib;
    this.listeners = {};
  }

  once(eventType, handler) {
    this.listeners[eventType] = handler;
  }

  run() {
    this.lib.groov_run_loop_step();
    const stack = this.lib.groov_read_incoming_events();

    for (let eventIndex = 0; eventIndex < stack.deref().len; ++eventIndex) {
      const event = stack.deref().events.deref()[`i${eventIndex}`];

      if (event.type in this.listeners) {
        this.listeners[event.type](event);
        delete this.listeners[event.type];
      }

      if (this.stopped) break;
    }
  }

  start() {
    this.intervalId = setInterval(this.run.bind(this), 0);
  }

  stop() {
    setTimeout(() => {
      this.stopped = true;
      clearInterval(this.intervalId);
    }, 100);
  }
}

describe('socket loop', () => {

  const HOST_ADDRESS = '127.0.0.1';
  const HOST_NAME = 'localhost';
  const HOST_PORT = 3000;
  let lib;
  let config;
  let eventLoop;

  beforeEach(() => {
    const hostNameBuf = new Buffer(HOST_ADDRESS);
    const hostAddressBuf = new Buffer(HOST_ADDRESS);

    const hostAddress = new uv_buf_t({base: hostAddressBuf, len: hostAddressBuf.length});
    const hostName = new uv_buf_t({base: hostNameBuf, len: hostNameBuf.length});

    config = new groov_config_t({host_name: hostName.ref(), host_address: hostName.ref(), port: HOST_PORT});

    lib = ffi.Library(
      path.join(__dirname, '..', 'build', 'Release', 'groov.so'), {
        'groov_init': ["void", [ref.refType(groov_config_t)]],
        'groov_read_incoming_events': [ref.refType(groov_event_stack_t), []],
        'groov_run_loop_step': ['void', []],
      }
    );

    eventLoop = new EventLoop(lib);
  });

  it('should connect to server', (done) => {
    const server = net.createServer(() => {
      server.close();
      done();
    }).listen(HOST_PORT);

    lib.groov_init(config.ref());
  });

  it('should read a connect event', (done) => {
    const server = net.createServer(() => {}).listen(HOST_PORT);

    lib.groov_init(config.ref());

    eventLoop.once(GROOV_EVENT_TYPE_CONNECT, () => {
      eventLoop.stop();
      server.close();
      done();
    });

    eventLoop.start();
  });


  it('should send an handshake request', (done) => {
    const io = socketIO();

    io.listen(HOST_PORT);
    lib.groov_init(config.ref());

    io.on('connect', () => {
      eventLoop.stop();
      io.close();
      done();
    });

    eventLoop.start();
  });

  it('should received an handshake event', (done) => {
    const io = socketIO();

    io.listen(HOST_PORT);
    lib.groov_init(config.ref());

    eventLoop.once(GROOV_EVENT_TYPE_HANDSHAKE, () => {
      eventLoop.stop();
      io.close();
      done();
    });

    eventLoop.start();
  });

  it('should received an io/connect event', (done) => {
    const io = socketIO();

    io.listen(HOST_PORT);
    lib.groov_init(config.ref());

    eventLoop.once(GROOV_EVENT_TYPE_IO_OPEN, () => {
      eventLoop.stop();
      io.close();
      done();
    });

    eventLoop.start();
  });
});
