/* -*- coding: utf-8 -*- */

/** 
 * Un monde graphique
 */

#include <iostream>                       // std::cout

#include <gl_window.hpp>

//******************************************************************************
int main( int argc, char *argv[] )
{
  std::cout << "__WINDOW" << std::endl;
  GLWindow app;

  std::cout << "__CREATION" << std::endl;
  app.init();

  app.render();

  return 0;
}
