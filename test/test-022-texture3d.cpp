/* -*- coding: utf-8 -*- */

/** 
 * TODO
 */

#include <3d/gl_3dapp.hpp>
#include <3d/gl_3dcanvas.hpp>
#include <3d/texture_screen.hpp>
#include <3d/glow_screen.hpp>
#include <3d/planet_screen.hpp>

int main(int argc, char *argv[])
{
  // test for argument
  if (argc != 2) {
    std::cout << "usage: " << argv[0] << " tex | glow | planet " << std::endl;
    exit(1);
  }
  
  GL3DApp app;
  GL3DCanvas canvas;
  GL3DScreen* screen;

  // tex -> TextureScreen
  if ( strcmp( argv[1], "tex" ) == 0 ) {
   screen = new TextureScreen( canvas );
  }
  else if ( strcmp( argv[1], "glow" ) == 0 ) {
    screen = new GlowScreen( canvas );
  }
  else if ( strcmp( argv[1], "planet" ) == 0 ) {
    screen = new PlanetScreen( canvas );
  }
  else {
    std::cout << argv[0] << ": argument '" << argv[1] << "' not recognized " << std::endl;
    exit(2);
  }
  
  app.run( screen );
  delete screen;
  
  return 0;
}
