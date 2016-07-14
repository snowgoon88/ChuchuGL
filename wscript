#!/usr/bin/env python
# encoding: utf-8

VERSION='0.0.1'
APPNAME='tactiship'

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
    ## Required LIBRARIES freetype2 using wrapper around pkg-config
    conf.check_cfg(package='freetype2',
                   uselib_store='FREETYPE2',
                   args=['--cflags', '--libs']
    )
    # ## Required LIBRARIES glib-2.0using wrapper around pkg-config
    # conf.check_cfg(package='glib-2.0',
    #                uselib_store='GLIB2',
    #                args=['--cflags', '--libs']
    # )
    # ## Required LIBRARIES Graphite2 using wrapper around pkg-config
    # conf.check_cfg(package='graphite2',
    #                uselib_store='GRAPHITE2',
    #                args=['--cflags', '--libs']
    # )

def build(bld):
    bld.recurse('src/tactiship')
    bld.recurse('test')


