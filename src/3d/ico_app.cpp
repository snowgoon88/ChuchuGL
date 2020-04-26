/* -*- coding: utf-8 -*- */

/** 
 * Example of how to run a <<Screen>> in a <<GL3DApp>>.
 */

#include <3d/gl_3dapp.hpp>
#include <3d/gl_3dcanvas.hpp>
#include <3d/ico_screen.hpp>

int main(int argc, char *argv[])
{
  GL3DApp app;
  GL3DCanvas canvas;
  GL3DScreen* screen = new IcoScreen( canvas );
  app.run( screen );
  
  return 0;
}
