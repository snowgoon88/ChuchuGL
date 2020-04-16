/* -*- coding: utf-8 -*- */

/** 
 * Application with a GL3DEngine and GL3DSolar.
 *
 */
#include <iostream>
#include <iomanip>        // set_precision(n)

#include <gl_3dengine.hpp>
#include <gl_3dsolar.hpp>
//******************************************************************************
int main( int argc, char *argv[] )
{
  std::cout << "__WINDOW" << std::endl;
  GL3DEnginePtr app = GL3DEnginePtr(new GL3DEngine( "Solar", 800, 800));
  
  std::cout << "__START_SCREEN" << std::endl;
  GL3DSolar solar_screen( app );
  
  std::cout << "__GO" << std::endl;
  app->run<GL3DSolar,bool>(  solar_screen);
  
  return 0;
}
