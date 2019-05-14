/* -*- coding: utf-8 -*- */

/** 
 * Use GLWindow with _key_debug
 */

#include <iostream>                       // std::cout
#include <gl_window.hpp>

// ******************************************************************** global
GLWindow *_win;

// ******************************************************************** render
void render()
{  
}

// ********************************************************************** main
int main( int argc, char *argv[] )
{
  // trying to set wcout to locale
  constexpr char locale_name[] = "";
  std::locale::global(std::locale(locale_name));
  std::wcout.imbue(std::locale());
  
  std::cout << "__WINDOW é" << std::endl;
  std::wcout << "__WINDOW utf8" << std::endl;
  
  _win = new GLWindow( "GL Console", 600, 480);
  _win->init();
  _win->_key_debug = true;

  // init timer
  //glfwSetTime( 0.0 );
  _win->run( render );

  // tidy up
  delete _win;
  
  return 0;
}
