/* -*- coding: utf-8 -*- */

/** 
 * Test displaying a list of oriented tokens.
 */
#include <iostream>                       // std::cout

#include <tactiship/gl_3dengine.hpp>
#include <gl_boids_screen.hpp>

int main(int argc, char *argv[])
{
  std::cout << "__WINDOW" << std::endl;
  GL3DEnginePtr app = GL3DEnginePtr(new GL3DEngine("Boids", 800, 600));
  
  std::cout << "__BOIDS_SCREEN" << std::endl;
  GLBoidsScreen boids_screen( app );
  
  std::cout << "__GO" << std::endl;
  app->run<GLBoidsScreen,bool>( boids_screen );

  return 0;
}
