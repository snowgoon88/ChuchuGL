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

    ## Required LIBRARIES OpenGL
    conf.check_cfg(package='gl',
                   uselib_store='OPENGL',
                   args=['--cflags', '--libs']
    )
    ## Required LIBRARIES GLFW3
    conf.check_cfg(package='glfw3',
                   uselib_store='GLFW3',
                   args=['--cflags', '--libs']
    )

def build(bld):
    bld.recurse('src/tactiship')


