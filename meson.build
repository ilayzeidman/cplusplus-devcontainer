project('mypassthrough', 'cpp', version : '1.0')

gst_dep = dependency('gstreamer-1.0')
gstbase_dep = dependency('gstreamer-base-1.0')

library('gstmypassthrough',
        'mypassthrough.cpp',
        dependencies : [gst_dep, gstbase_dep],
        install : true,
        install_dir : join_paths(get_option('libdir'), 'gstreamer-1.0'))
