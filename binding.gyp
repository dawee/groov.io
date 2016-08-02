{
  'targets': [
    {
      'dependencies': ['./deps/libuv/uv.gyp:libuv'],
      'target_name': 'libunio',
      'type': 'shared_library',
      'sources': [
        './src/boot.c',
        './src/event.c',
        './src/incoming.c',
        './src/loop.c',
        './src/outgoing.c',
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
