/* -*- coding: utf-8 -*- */

/** 
 * TODO
 */

#include <3d/gl_3dapp.hpp>
#include <3d/gl_3dcanvas.hpp>
#include <3d/texture_screen.hpp>

int main(int argc, char *argv[])
{
  GL3DApp app;
  GL3DCanvas canvas;
  GL3DScreen* screen = new TextureScreen( canvas );
  app.run( screen );
  
  return 0;
}
