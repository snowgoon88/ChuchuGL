/* -*- coding: utf-8 -*- */

/** 
 * Test le GL3DScreen avec une tête de flèche.
 */

#include <iostream>                       // std::cout

#include <gl_engine.hpp>
#include <gl_3dscreen.hpp>

// Model

//******************************************************************************
int main( int argc, char *argv[] )
{
  std::cout << "__WINDOW" << std::endl;
  GLEngine app( "TactiShip", 640, 480);
  
  std::cout << "__START_SCREEN" << std::endl;
  GL3DScreen trid_screen( app );
  
  std::cout << "__GO" << std::endl;
  app.run<GL3DScreen,bool>( trid_screen );
  
  return 0;
}
