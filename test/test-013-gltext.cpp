/* -*- coding: utf-8 -*- */

/** 
 * Teste GLTextShaders
 */
// OpenGL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>

#include <gl_text_shaders.hpp>

// ******************************************************************** Global
GLFWwindow* _window;
int _screen_width=800, _screen_height=600;

/**
 * callback pour gérer les messages d'erreur de GLFW
 */
static void error_callback(int error, const char* description)
{
  std::cerr <<  description << std::endl;
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  // ESC
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(_window, GL_TRUE);
};
// ********************************************************************** main
int main( int argc, char *argv[] )
{
  //TODO Ouvrir une fenêtre basique avec GLFWT
  if (!glfwInit())
    exit(EXIT_FAILURE);
  glfwSetErrorCallback(error_callback);

  _window = glfwCreateWindow(_screen_width, _screen_height,
			     "Essai GLTextShaders", NULL, NULL);
  if (!_window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  // set key callback
  glfwSetKeyCallback(_window, key_callback);
  
  /* Make the window's context current */
  glfwMakeContextCurrent(_window);

  GLTextShaders gltext;
  gltext.set_size( 16 );
  
  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(_window))
    {
      glfwGetFramebufferSize(_window, &_screen_width, &_screen_height);
      float sx = 2.0 / _screen_width;
      float sy = 2.0 / _screen_height;
      
      /* Render here */
      glClearColor( 0,0,0,1 );
      glClear(GL_COLOR_BUFFER_BIT);

      gltext.pre_render();
      gltext.set_color( {1,1,1,1} );
      gltext.set_size( 16 );
      gltext.render_text("Cette phrase est en blanc",
			 -1 + 8 * sx, 1 - 50 * sy,
			 sx, sy);
      gltext.set_size( 40 );
      gltext.set_color( {0,0,1,1} );
      gltext.render_text("Celle-la est bleu, enfin normalement...",
			 -1 + 16 * sx, 1 - 100 * sy,
			 sx, sy);
      gltext.set_size( 16 );
      gltext.set_color( {1,0,0,1} );
      gltext.render_text("Et hop du rouge énervé",
			 -1 + 16 * sx, 1 - 150 * sy,
			 sx, sy);
      gltext.render_text(u"Et hop du rouge énervé",
			 -1 + 16 * sx, 1 - 180 * sy,
			 sx, sy);
      
      gltext.post_render();

      
      /* Swap front and back buffers */
      glfwSwapBuffers(_window);
      
      /* Poll for and process events */
      glfwPollEvents();
    }
  
  glfwTerminate();
  return 0;
}
