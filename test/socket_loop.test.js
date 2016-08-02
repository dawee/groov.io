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

const unio_config_t = Struct();
unio_config_t.defineProperty('host_address', ref.refType(uv_buf_t));
unio_config_t.defineProperty('port', ref.types.int);

const unio_event_t = Struct();
unio_event_t.defineProperty('name', ref.refType(uv_buf_t));
unio_event_t.defineProperty('data', ref.refType(uv_buf_t));

const unio_event_stack_t = Struct();

unio_event_stack_t.defineProperty('len', ref.types.int);
unio_event_stack_t.defineProperty('events', ref.refType(unio_event_t));


describe('socket loop', () => {

  const HOST_ADDRESS = '127.0.0.1';
  const HOST_PORT = 3000;
  let lib;
  let config;

  beforeEach(() => {
    const hostNameBuf = new Buffer(HOST_ADDRESS);
    const hostName = new uv_buf_t({base: hostNameBuf, len: hostNameBuf.length});
    
    config = new unio_config_t({host_address: hostName.ref(), port: HOST_PORT}); 

    lib = ffi.Library(
      path.join(__dirname, '..', 'out', 'Default', 'obj.target', 'libunio.so'), {
        'unio_init': ["void", [ref.refType(unio_config_t)]],
        'unio_read_incoming_events': ["int", []],
      }
    );
  });

  it('should connect to server', (done) => {
    const io = socketIO();

    const server = net.createServer(() => {
      server.close();
      done();
    }).listen(HOST_PORT);

    lib.unio_init(config.ref());
  });

  it('should read a connect event', (done) => {
    const io = socketIO();

    const server = net.createServer(() => {}).listen(HOST_PORT);

    lib.unio_init(config.ref());
    setInterval(() => {
     const count = lib.unio_read_incoming_events();

     if (count === 1) {
      server.close();
      done();
     }
    }, 10);
  });
});
