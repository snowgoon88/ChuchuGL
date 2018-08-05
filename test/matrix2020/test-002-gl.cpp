/* -*- coding: utf-8 -*- */

/** 
 * test-002.-g.cpp
 * 
 * Display Environment trough GLEnvironment
 */

#include <gl_window.hpp>

#include <matrix2020/gl_environment.hpp>
using namespace matrix2020;

#include <iostream>                       // std::cout


GLWindow* _win;
GLEnvironment* _gl_env;

void render()
{
  _gl_env->render();
}

//******************************************************************************
int main( int argc, char *argv[] )
{
  std::cout << "__ENVIRONMENT" << std::endl;
  Environment _env;
  _env.load_from_txt( "data/matrix00.txt" );
  std::cout << _env.str_dump() << std::endl;

  std::cout << "__WINDOW" << std::endl;
  _win = new GLWindow( "GL Window", 640, 480);


  _gl_env = new GLEnvironment( _env ); 
  
  _win->run( render );

  delete _gl_env;
  delete _win;
  
  return 0;
}
