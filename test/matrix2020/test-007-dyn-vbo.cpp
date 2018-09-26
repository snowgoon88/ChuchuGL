/* -*- coding: utf-8 -*- */

/** 
 * Implement a dymamic VBO with list of vertex that are origin of QUADS
 * with RGBA colors drawn using a geometry shader.
 *
 * In a first time : 2 square moving right :o)
 */

/* 
https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming
http://forum.lwjgl.org/index.php?topic=5334.0
https://www.opengl.org/discussion_boards/showthread.php/178828-VBO-with-dynamically-changing-number-of-points-%21%21/page2
http://www.songho.ca/opengl/gl_vbo.html#example3
*/

#include <iostream>                       // std::cout

#include <gl_window.hpp>
#include <matrix2020/gl_fovhamming.hpp>
using namespace matrix2020;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

GLWindow* _win = nullptr;
GLFovHamming* _gl_fov = nullptr;

void render()
{
  auto now = glfwGetTime();
  if (now > 5.0) {
    glfwSetTime( 0.0 );
  }
  _gl_fov->update_data( now );
  _gl_fov->render();
}


int main(int argc, char *argv[])
{
  std::cout << "__WINDOW" << std::endl;
  _win = new GLWindow( "GL Window", 640, 480);

  std::cout << "__GL_FOV" << std::endl;
  _gl_fov = new GLFovHamming();

  // start timer
  glfwSetTime( 0.0 );
  _win->run( render );

  delete _gl_fov;
  delete _win;
  
  return 0;
}

