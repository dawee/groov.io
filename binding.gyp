{
  'targets': [
    {
      'dependencies': ['./deps/libuv/uv.gyp:libuv'],
      'target_name': 'libgroov',
      'type': 'shared_library',
      'include_dirs': [
        './deps/slre',
      ],
      'sources': [
        './deps/slre/slre.c',
        './src/boot.c',
        './src/event.c',
        './src/handshake.c',
        './src/incoming.c',
        './src/io.c',
        './src/logger.c',
        './src/loop.c',
        './src/outgoing.c',
        './src/packet.c',
        './src/state.c',
      ],
      'conditions': [
        ['OS=="win"', {
          'defines': ['HAVE_UNISTD_H=0'],
          'sources': ['getopt.c']
        }, {
          'defines': ['HAVE_UNISTD_H=1']
        }]
      ]
    }
  ]
}
