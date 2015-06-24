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
  app.set_controler( GLControler::Type::KEYBOARD );
  // app.set_controler( GLControler::Type::XPAD );

  app.render();

  return 0;
}
