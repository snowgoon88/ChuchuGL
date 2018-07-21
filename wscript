#!/usr/bin/env python
# encoding: utf-8

## Par défaut ./waf configure va utiliser buildir=wbuild et CXX=g++
## MAIS avec ./waf configure --out=cbuild --check-cxx-compiler=clang++
##      on peut utilise clang :o)
## l'option --color permet de coloriser la sortie du compilo

# wscript in $CHUCHU

## Pour avoir accès au CORE de WAF
import waflib.Options as wo

VERSION='0.0.2'
APPNAME='chuchu'

top = '.'
## By default, buildir is set to 'wbuild'.
##   can be changed with --out=cbuild (par exemple)
out = 'wbuild'

## une Command pour WAF a toujours un contexte, qu'on peut appeler comme
## on veut (ctx, bld, opt, ...)
def info(ctx):
    ## les options sont toujours lues avant
    print( "** INFO **")
    print( "top=",top )
    print( "out=",out )
    print( "OPT=",wo.options )
    print( "clang=",ctx.options.clang )

## la varialble ctx.env persiste entre les contextes.
##    on peut y stocker des valeurs et récupérer par ${VAR}
##    exemple : opt.env.BAR = 2 ,${BAR} est défini dans scripts
##    caché dans [wc]build/c4che/_cache.py
def options(opt):
    print('→ options from ' + opt.path.abspath())
    ## Ajouter option 'clang', False par default
    ##opt.add_option('--clang', action='store_true', default=False, help='Use clang compiler')
    ## C++ ready (gcc g++)
    opt.load('compiler_cxx')

def configure(conf):
    global out
    print('→ config from ' + conf.path.abspath())
    ##conf.env.CLANG = conf.options.clang
    ## Change build dir if using clang
    ##if conf.options.clang:
    ##    out = 'cbuild'
    ##print("  done setting CLANG")
    conf.load('compiler_cxx')
    print( "CXX=",conf.env.CXX)
    ##conf.env.cxxflags = ['-D_REENTRANT','-Wall','-fPIC','-g','-std=c++11']
    ##conf.env.includes = ['src']
    ##conf.check_tool('compiler_cxx')
    conf.env['CXXFLAGS'] = ['-D_REENTRANT','-Wall','-fPIC','-g','-std=c++11']
    ## option --color le fair
    # if '/usr/bin/clang++' in conf.env.CXX:
    #     conf.env['CXXFLAGS'].append( '-fcolor-diagnostics' )
    # else:
    #     conf.env['CXXFLAGS'].append( '-fdiagnostics-color=always' )
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
    print('→ build from ' + bld.path.abspath() + " with CXX=" + str(bld.env.CXX))
    print('  CXXFLAGS=' + str(bld.env.CXXFLAGS) )
    bld.recurse('src')
    bld.recurse('src/tactiship')
    bld.recurse('src/boid')
    bld.recurse('test')
    bld.recurse('test/matrix2020')    

