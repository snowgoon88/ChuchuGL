/* -*- coding: utf-8 -*- */

/** 
 * Test le GLEngine par défaut.
 */

#include <iostream>                       // std::cout

#include <gl_engine.hpp>
#include <gl_screen.hpp>

//******************************************************************************
int main( int argc, char *argv[] )
{
  std::cout << "__WINDOW" << std::endl;
  GLEngine app( "Chuchu Rocket", 640, 480);

  std::cout << "__SCREEN" << std::endl;
  GLScreen start_screen( app );
  
  std::cout << "__GO" << std::endl;
  app.run<GLScreen,bool>( start_screen );

  return 0;
}
