#!/usr/bin/env python
# encoding: utf-8

## Par défaut ./waf configure va utiliser buildir=wbuild et CXX=g++
## MAIS avec ./waf configure --out=cbuild --check-cxx-compiler=clang++
##      on peut utilise clang :o)
## l'option --color permet de coloriser la sortie du compilo

# wscript in $CHUCHU

## Pour copier les dir
import os
import shutil

VERSION='0.0.2'
APPNAME='chuchu'

top = '.'
## By default, buildir is set to 'wbuild'.
##   can be changed with --out=cbuild (par exemple)
out = 'wbuild'

opt_flags = '-O3'
debug_flags = '-O0 -g'

# ************************************************************************* help
def help(ctx):
    print( "**** WAF for VisuGL, usual commands ************" )
    print( "configuration :  ./waf configure --out=cbuild [--use_clang => --check_cxx_compiler=clang++]" )
    print( "                       --prefix=[/usr/local]" )
    print( "build :          ./waf build ")
    print( "build specific : ./waf build --targets=test/001-curve" )
    print( "install :        ./waf install (in --prefix) " )
    print( "clean :          ./waf clean" )
    print( "detailed help :  ./waf --help or see https://waf.io/book" )
    print( "  options :      --use_clang --compil_db --atb --debug" )
# ********************************************************************** options
def options( opt ):
    """
    opt : OptionContext
    options use the optparse https://docs.python.org/3/library/optparse.html
    """
    opt.load( 'compiler_cxx' )

    # option use_clang
    opt.add_option('--use_clang', dest='use_clang', action="store_true", default=False,
                   help='use clang and compile in cbuild (replace --out --check_cxx_compiler)' )

    # option debug
    opt.add_option('--debug', dest='debug', action="store_true", default=False,
                   help='compile with debugging symbols' )

    # clang compilation database
    opt.add_option('--compil_db', dest='compil_db', action="store_true", default=False,
                   help='use clang compilation database' )

    # define some macro for C++
    # (equivalent to #define LABEL or -DLABEL to compilator
    opt.add_option('-D', '--define', action="append", dest="defined_macro",
                   help='define preprocessing macro' )


# **************************************************************** CMD configure
def configure(conf):
    print( "__CONFIGURE" )
    print( '→ config from ' + conf.path.abspath())

    #memorise appname as it is not easily recoverable later
    conf.env.appname = APPNAME
    conf.env.version = VERSION

    if conf.options.use_clang:
        conf.options.check_cxx_compiler = "clang++"
        # 'out' cannot be without option --out as it is checked BEFORE reading wscript

    ## set env and options according to the possible compilers
    ## better to set options 'out' and 'check_cxx_compiler' before loading
    conf.load( 'compiler_cxx' )

    # sys.exit()
    if conf.options.compil_db:
        ## To generate 'compile_commands.json' at the root of buildpath
        # to be linked/copied in order to user 'cquery' in emacs through lsp-mode
        # see https://github.com/cquery-project/cquery/wiki/Emacs
        conf.load('clang_compilation_database', tooldir="ressources")
        print( "CXX=",conf.env.CXX)

    conf.env['CXXFLAGS'] = ['-D_REENTRANT','-Wall','-fPIC','-std=c++11']

    ## Check GLM
    #conf.check_cc(header_name="glm/glm.hpp")

    ## Require libglm, using wrapper around pkg-config
    conf.check_cfg(package='glm',
                   uselib_store='GLM',
                   args=['--cflags', '--libs']
    )
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
    ## Require/Check libncurses
    conf.check_cfg(package='ncurses',
                   uselib_store='NCURSES',
                   args=['--cflags', '--libs']
    )
    ## Required LIBRARIES freetype2 using wrapper around pkg-config
    conf.check_cfg(package='freetype2',
                   uselib_store='FREETYPE2',
                   args=['--cflags', '--libs']
    ) 
    ## Require/Check libboost
    conf.env.LIB_BOOST = ['boost_program_options']
    conf.env.LIBPATH_BOOST = ['/usr/lib/x86_64-linux-gnu','/usr/lib/i386-linux-gnu']
    print("Checking for 'BOOST::program_options'")
    conf.find_file( 'lib'+conf.env.LIB_BOOST[0]+'.so', conf.env.LIBPATH_BOOST )
    ## Require/Check libstb
    conf.check_cfg(package='stb',
                   uselib_store='STB',
                   args=['--cflags', '--libs']
    )

# ******************************************************************** CMD build
def build(bld):
    print('→ build from ' + bld.path.abspath() + " with CXX=" + str(bld.env.CXX))
    print('  CXXFLAGS=' + str(bld.env.CXXFLAGS) )

    # check debug option
    if bld.options.debug:
        bld.env['CXXFLAGS'] += debug_flags.split(' ')
    else:
        bld.env['CXXFLAGS'] += opt_flags.split(' ')
    print( bld.env['CXXFLAGS'] )

    # add macro defines as -D options
    # print( "MACRO=", bld.options.defined_macro )
    if bld.options.defined_macro:
        # print( "ENV=", bld.env.DEFINES )
        bld.env.DEFINES = bld.env.DEFINES + bld.options.defined_macro
        # print( "new ENV=", bld.env.DEFINES )


    bld.recurse('src')
    bld.recurse('src/tactiship')
    bld.recurse('src/boid')
    bld.recurse('src/matrix2020')
    bld.recurse('test')
    bld.recurse('test/matrix2020')

    def copy_data(bld):
        src = bld.srcnode.abspath()+'/data'
        tgt = bld.bldnode.abspath()+'/data'
        copydir_ifneeded( src, tgt )
        
    def copy_shaders(bld):
        src = bld.srcnode.abspath()+'/src/shaders'
        tgt = bld.bldnode.abspath()+'/src/shaders'
        copydir_ifneeded( src, tgt )

    def copy_ressources(bld):
        src = bld.srcnode.abspath()+'/ressources'
        tgt = bld.bldnode.abspath()+'/ressources'
        copydir_ifneeded( src, tgt )
    
    bld.add_post_fun( copy_data )
    bld.add_post_fun( copy_shaders )
    bld.add_post_fun( copy_ressources )


def copydir_ifneeded( src, tgt ):
    # tgt does not exists => copy
    if not os.access( tgt, os.F_OK ):
        print( "__copy "+src+" to "+tgt )
        shutil.copytree(src, tgt, symlinks=True)
    else:
        print( "__check each file of "+src )
        names = os.listdir(src)
        for name in names:
            srcname = os.path.join(src, name)
            tgtname = os.path.join(tgt, name)

            if not os.access( tgtname, os.F_OK ):
                print( "  copy "+name )
                shutil.copy2(srcname, tgtname)
            else:
                st_src = os.stat(srcname)
                st_tgt = os.stat(tgtname)

                # same size and identical timestamps -> make no copy
                if st_tgt.st_mtime + 2 < st_src.st_mtime or st_src.st_size != st_tgt.st_size:
                    print( "  copy "+name )
                    shutil.copy2(srcname, tgtname)
    
