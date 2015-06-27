/* -*- coding: utf-8 -*- */

/** 
 * Un monde graphique
 */

#include <iostream>                       // std::cout

#include <gl_window.hpp>
#include <gl_controler.hpp>

//******************************************************************************
int main( int argc, char *argv[] )
{
  std::cout << "__WINDOW" << std::endl;
  GLWindow app;

  std::cout << "__CREATION" << std::endl;
  app.init();

  std::cout << "__SET_CONTROLER" << std::endl;
  // app.set_controler( 0 );
  app.set_controler( 1 );
  // app.set_controler( 2 );

  app.render();

  return 0;
}
