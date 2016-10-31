#!/usr/bin/env python
# encoding: utf-8

# wscript in $CHUCHU

VERSION='0.0.2'
APPNAME='chuchu'

top = '.'
out = 'wbuild'

def options(opt):
    opt.load('compiler_cxx')


def configure(conf):
    conf.load('compiler_cxx')
    ##conf.env.cxxflags = ['-D_REENTRANT','-Wall','-fPIC','-g','-std=c++11']
    ##conf.env.includes = ['src']
    ##conf.check_tool('compiler_cxx')
    conf.env['CXXFLAGS'] = ['-D_REENTRANT','-Wall','-fPIC','-g','-std=c++11']
    ##conf.env['INCLUDES'] = ['src']

    ## Check GLM
    conf.check_cc(header_name="glm/glm.hpp")

    ## Required LIBRARIES OpenGL using wrapper around pkg-config
    conf.check_cfg(package='gl',
                   uselib_store='OPENGL',
                   args=['--cflags', '--libs']
    )
    ## Required LIBRARIES GLFW3 using wrapper around pkg-config
    conf.check_cfg(package='glfw3',
                   uselib_store='GLFW3',
                   args=['--cflags', '--libs']
    )
    ## Required LIBRARIES FTGL
    conf.check_cfg(package='ftgl',
                   uselib_store='FTGL',
                   args=['--cflags', '--libs']
    )
    ## Required LIBRARIES PortAudio
    conf.check_cfg(package='portaudio-2.0',
                   uselib_store='PA',
                   args=['--cflags', '--libs']
    )
    ## Required LIBRARIES Vorbis
    conf.check_cfg(package='vorbisfile',
                   uselib_store='VORBIS',
                   args=['--cflags', '--libs']
    )
    ## Require/Check libSOIL
    conf.check_cxx( lib="SOIL" )
    ## Required LIBRARIES freetype2 using wrapper around pkg-config
    conf.check_cfg(package='freetype2',
                   uselib_store='FREETYPE2',
                   args=['--cflags', '--libs']
    )
    ## Require/Check libboost
    conf.env.LIB_BOOST = ['boost_program_options']
    conf.env.LIBPATH_BOOST = ['/usr/lib/x86_64-linux-gnu','/usr/lib/i386-linux-gnu']
    print "Checking for 'BOOST::program_options'"
    conf.find_file( 'lib'+conf.env.LIB_BOOST[0]+'.so', conf.env.LIBPATH_BOOST )

def build(bld):
    print('→ build from ' + bld.path.abspath())
    bld.recurse('src')
    bld.recurse('src/tactiship')
    bld.recurse('test')    


