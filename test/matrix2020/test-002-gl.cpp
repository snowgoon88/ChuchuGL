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

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

GLWindow* _win;
GLEnvironment* _gl_env;
glm::mat4 _proj, _view, _projview;

void render()
{
  _gl_env->render( _projview );
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
  
  // Camera mode : static
  _proj = glm::mat4( 1.0f );
  _proj = glm::ortho( -3.f, 15.f, // left;right
                      -3.f, 11.f, // bottom,top
                      -1.f, 100.f // near far
                      );
  _view = glm::mat4( 1.0f );
  _view = glm::translate(_view, glm::vec3(0.0f, 0.0f, -10.0f));
  _projview = _proj * _view;
  _win->run( render );

  delete _gl_env;
  delete _win;
  
  return 0;
}
