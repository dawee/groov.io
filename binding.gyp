{
  'targets': [
    {
      'dependencies': ['./deps/libuv/uv.gyp:libuv'],
      'target_name': 'libgroov',
      'type': 'shared_library',
      'sources': [
        './deps/slre/slre.c',
        './src/boot.c',
        './src/event.c',
        './src/incoming.c',
        './src/loop.c',
        './src/outgoing.c',
        './src/parser.c',
        './src/serializer.c',
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
