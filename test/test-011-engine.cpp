/* -*- coding: utf-8 -*- */

/** 
 * Test le GLEngine par défaut.
 */

#include <iostream>                       // std::cout

#include <gl_engine.hpp>
#include <gl_screen.hpp>
#include <gl_controler.hpp>

// Model
#include <world.hpp>

//******************************************************************************
int main( int argc, char *argv[] )
{
  std::cout << "__WINDOW" << std::endl;
  GLEngine app( "Chuchu Rocket", 640, 480);

  std::cout << "__START_SCREEN" << std::endl;
  GLScreen start_screen( app );
  
  std::cout << "__GO" << std::endl;
  bool start = app.run<GLScreen,bool>( start_screen );
  
  if( start ) {
    World world;
    GLControler controler_screen( app, world );
    bool ready = app.run<GLControler,bool>( controler_screen );
  } 
    
  return 0;
}
