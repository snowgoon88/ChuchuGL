/* -*- coding: utf-8 -*- */

/** 
 * test-003-agent.cpp
 * Display some agentss...
 */

#include <gl_window.hpp>

#include <iostream>                       // std::cout

#include <list>
#include <matrix2020/m_gameobject.hpp>
#include <matrix2020/gl_agent.hpp>
using namespace matrix2020;

using GameObjectPtrC = std::list<GameObjectPtr>;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

GLWindow* _win = nullptr;
GameObjectPtrC _agents;
GLAgent* _gl_agent = nullptr;

glm::mat4 _proj, _view, _projview;

void render()
{
  auto it = _agents.begin();
  (*it)->orient( AllDir[static_cast<int>(glfwGetTime())%AllDir.size()] );
  _gl_agent->render( _projview );
}

int main(int argc, char *argv[])
{
  std::cout << "__WINDOW" << std::endl;
  _win = new GLWindow( "GL Window", 640, 480);

  std::cout << "__AGENTS" << std::endl;
  auto ag1 = GameObjectPtr( new GameObject( "ag1" ));
  ag1->pos( {2, 1} );
  ag1->orient( D_UP );
  _agents.push_back( ag1 );
  
  std::cout << "__GL_AGENT" << std::endl;
  _gl_agent = new GLAgent( _agents );

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

  delete _gl_agent;
  delete _win;
  
  
  return 0;
}

