/* -*- coding: utf-8 -*- */

/** 
 * test-003-hacker.cpp
 * Display some hackers...
 */

#include <gl_window.hpp>

#include <iostream>                       // std::cout

#include <matrix2020/m_gameobject.hpp>
#include <matrix2020/gl_hacker.hpp>
using namespace matrix2020;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

GLWindow* _win = nullptr;
GameObjectPtr _go = nullptr;
GLHacker* _gl_hacker = nullptr;

void render()
{
  _gl_hacker->render();
}

int main(int argc, char *argv[])
{
  std::cout << "__WINDOW" << std::endl;
  _win = new GLWindow( "GL Window", 640, 480);

  std::cout << "__HACKER" << std::endl;
  _go = GameObjectPtr( new GameObject( "h01" ));
  _go->_pos = {1, 0};
  std::cout << "__GL_HACKER" << std::endl;
  _gl_hacker = new GLHacker( _go );

  _win->run( render );

  delete _gl_hacker;
  delete _win;
  
  
  return 0;
}

