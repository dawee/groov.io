const ffi = require('ffi');
const ref = require('ref');
const Struct = require('ref-struct');
const path = require('path');
const socketIO = require('socket.io');
const net = require('net');


/*
 * C Structs
 */

const unio_text_t = Struct();
unio_text_t.defineProperty('base', ref.types.CString);
unio_text_t.defineProperty('len', ref.types.size_t);

const unio_config_t = Struct();
unio_config_t.defineProperty('host_address', ref.refType(unio_text_t));
unio_config_t.defineProperty('port', ref.types.int);

const unio_event_t = Struct();
unio_event_t.defineProperty('name', ref.refType(unio_text_t));
unio_event_t.defineProperty('data', ref.refType(unio_text_t));

const unio_event_stack_t = Struct();

unio_event_stack_t.defineProperty('len', ref.types.int);
unio_event_stack_t.defineProperty('events', ref.refType(unio_event_t));

/*
 * Helpers
 */

function createConfig(ctx, hostName, port) {
  ctx.hostNameBuf = new Buffer(hostName);
  ctx.hostName = new unio_text_t({base: ctx.hostNameBuf, len: ctx.hostNameBuf.length});
  ctx.config = new unio_config_t({host_address: ctx.hostName.ref(), port: port}); 
}


/*
 * Test Scenario
 */


describe('socket loop', () => {

  let lib;

  beforeEach(() => {
    lib = ffi.Library(
      path.join(__dirname, '..', 'out', 'Default', 'obj.target', 'libunio.so'), {
        'unio_init': ["void", [ref.refType(unio_config_t)]],
        'unio_read_events': ["void", []],
      }
    );
  });

  it('should connect to server', (done) => {
    const ctx = {};
    const io = socketIO();

    createConfig(ctx, "127.0.0.1", 3000);

    net.createServer(() => done()).listen(3000);

    lib.unio_init(ctx.config.ref());
    setInterval(lib.unio_read_events, 100);
  });

});
