/* -*- coding: utf-8 -*- */

/** 
 * Le fond d'écran d'invitation à jouer.
 */

#include <iostream>                       // std::cout

#include <gl_screen.hpp>

//******************************************************************************
int main( int argc, char *argv[] )
{
  std::cout << "__WINDOW" << std::endl;
  GLScreen app( "Chuchu Rocket", 640, 480);

  std::cout << "__CREATION" << std::endl;
  app.init();

  app.render();

  return 0;
}
