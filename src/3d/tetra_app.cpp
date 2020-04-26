/* -*- coding: utf-8 -*- */

/** 
 * Example of how to run a <<Screen>> in a <<GL3DApp>>.
 */

#include <3d/gl_3dapp.hpp>
#include <3d/gl_3dcanvas.hpp>
#include <3d/tetra_screen.hpp>

int main(int argc, char *argv[])
{
  GL3DApp app;
  GL3DCanvas canvas;
  TetraScreen tetra_screen( canvas );
  app.run( &tetra_screen );
  
  return 0;
}
