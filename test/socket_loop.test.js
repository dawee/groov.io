const ffi = require('ffi');
const ref = require('ref');
const Struct = require('ref-struct');
const path = require('path');
const socketIO = require('socket.io');
const net = require('net');


/*
 * C Structs
 */

const uv_buf_t = Struct();
uv_buf_t.defineProperty('base', ref.types.CString);
uv_buf_t.defineProperty('len', ref.types.size_t);

const groov_config_t = Struct();
groov_config_t.defineProperty('host_name', ref.refType(uv_buf_t));
groov_config_t.defineProperty('host_address', ref.refType(uv_buf_t));
groov_config_t.defineProperty('port', ref.types.int);

const groov_event_t = Struct();
groov_event_t.defineProperty('type', ref.types.int);
groov_event_t.defineProperty('data', ref.refType(uv_buf_t));

const groov_event_stack_t = Struct();
groov_event_stack_t.defineProperty('len', ref.types.int);
groov_event_stack_t.defineProperty('events', ref.refType(groov_event_t));


const GROOV_EVENT_TYPE_CONNECT = 1;


describe('socket loop', () => {

  const HOST_ADDRESS = '127.0.0.1';
  const HOST_NAME = 'localhost';
  const HOST_PORT = 3000;
  let lib;
  let config;

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
    const intervalId = setInterval(() => {
      lib.groov_run_loop_step();

      const stack = lib.groov_read_incoming_events();
      
      if (stack.deref().len == 1 && stack.deref().events.deref().type == GROOV_EVENT_TYPE_CONNECT) {
        server.close();
        clearInterval(intervalId);
        done();
      }
    }, 0);
  });


  it('should send an handshake request', (done) => {
    const io = socketIO();
    let intervalId;

    io.listen(HOST_PORT);

    lib.groov_init(config.ref());

    io.on('connect', () => {
      setTimeout(() => {
        io.close();
        clearInterval(intervalId);
        done();
      }, 50);
    });

    intervalId = setInterval(lib.groov_run_loop_step, 0);
  });

});
