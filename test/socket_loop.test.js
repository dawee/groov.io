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
groov_config_t.defineProperty('host_name', ref.types.CString);
groov_config_t.defineProperty('host_port', ref.types.int);



const groov_event_t = StructType();
groov_event_t.defineProperty('type', ref.types.int);
groov_event_t.defineProperty('data', ref.refType(uv_buf_t));

const groov_events_t = StructType();

for (let i = 0; i < 1000; ++i) {
  groov_events_t.defineProperty(`i${i}`, groov_event_t);
}


const groov_event_stack_t = StructType();
groov_event_stack_t.defineProperty('len', ref.types.int);
groov_event_stack_t.defineProperty('events', ref.refType(groov_events_t));


const groov_logging_event_t = StructType();
groov_logging_event_t.defineProperty('level', ref.types.char);
groov_logging_event_t.defineProperty('len', ref.types.uint);

for (let i = 0; i < 100; ++i) {
  groov_logging_event_t.defineProperty(`base-${i}`, ref.types.char);
}


const GROOV_EVENT_TYPE_CONNECT = 1;
const GROOV_EVENT_TYPE_HANDSHAKE = 2;
const GROOV_EVENT_TYPE_IO_OPEN = 3;

const GROOV_EVENT_TYPE_LOGGING = 200;

const HOST_NAME = 'localhost';
const HOST_PORT = 3000;


class EventLoop {

  constructor(lib) {
    const config = new groov_config_t({host_name: HOST_NAME, host_port: HOST_PORT});

    this.lib = lib;
    this.listeners = {};

    lib.groov_init(config.ref());
  }

  once(eventType, handler) {
    this.listeners[eventType] = handler;
  }

  run() {
    this.lib.groov_run_loop_step();
    const stack = this.lib.groov_read_incoming_events();

    for (let eventIndex = 0; eventIndex < stack.deref().len; ++eventIndex) {
      const event = stack.deref().events.deref()[`i${eventIndex}`];

      if (event.type == GROOV_EVENT_TYPE_LOGGING) {
        const logging_event = this.lib.groov_read_logging_event(event.ref()).deref();
        const charArray = [];

        for (let i = 0; i < logging_event.len && i < 100; ++i) {
          charArray.push(logging_event[`base-${i}`]);
        }

        console.log(new Buffer(charArray).toString());
      }


      if (event.type in this.listeners) {
        this.listeners[event.type](event);
        delete this.listeners[event.type];
      }

      if (this.stopped) break;
    }
  }

  start() {
    this.lib.groov_connect();
    this.intervalId = setInterval(this.run.bind(this), 0);
  }

  stop() {
    this.stopped = true;
    clearInterval(this.intervalId);
  }
}

describe('socket loop', () => {
  let lib;
  let eventLoop;

  beforeEach(() => {
    lib = ffi.Library(
      path.join(__dirname, '..', 'build', 'Release', 'groov.so'), {
        'groov_init': ["void", [ref.refType(groov_config_t)]],
        'groov_read_incoming_events': [ref.refType(groov_event_stack_t), []],
        'groov_read_logging_event': [ref.refType(groov_logging_event_t), [ref.refType(groov_event_t)]],
        'groov_run_loop_step': ['void', []],
        'groov_connect': ['void', []],
      }
    );

    eventLoop = new EventLoop(lib);
  });

  it('should connect to server', (done) => {
    const server = net.createServer(() => {
      eventLoop.stop();
      server.close();
      done();
    }).listen(HOST_PORT);

    eventLoop.start();
  });

  it('should read a connect event', (done) => {
    const server = net.createServer(() => {}).listen(HOST_PORT);

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
    eventLoop.once(GROOV_EVENT_TYPE_HANDSHAKE, () => {
      eventLoop.stop();
      io.close();
      done();
    });

    eventLoop.start();
  });

  it('should received an io/open event', (done) => {
    const io = socketIO();

    io.listen(HOST_PORT);
    eventLoop.once(GROOV_EVENT_TYPE_IO_OPEN, () => {
      eventLoop.stop();
      io.close();
      done();
    });

    eventLoop.start();
  });
});
