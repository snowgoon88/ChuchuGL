/* -*- coding: utf-8 -*- */

#ifndef GL_WINDOW_HPP
#define GL_WINDOW_HPP

/** 
 * Crée une fenêtre GLFW basique.
 * Sans shader
 */

// OpenGL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <GLFW/glfw3.h>

#include <memory>                        // std::*_ptr
// Viewer
#include <gl_texture.hpp>
#include <gl_texture_fade.hpp>

// ******************************************************************** GLOBAL
typedef std::unique_ptr<GLTexture> GLTexturePtr;
typedef std::unique_ptr<GLTextureFade> GLTextureFadePtr;

// ***************************************************************************
// ****************************************************************** GLWindow
// ***************************************************************************
class GLWindow
{
public:
  // ****************************************************** GLWindow::creation
  GLWindow(const std::string& title = "GL Window",
	   int width=800, int height=600,
	   bool fullsize=false) :
    _screen_width(width), _screen_height(height)
  {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
      std::cerr << "ERROR: Failed to init GLFW" << std::endl;
      exit(EXIT_FAILURE);
    }
    // opengl 3.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    

    if( fullsize ) {
      _window = glfwCreateWindow(width, height, title.c_str(),
				 glfwGetPrimaryMonitor(), NULL);
    }
    else {
      _window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    }
    
    if (!_window) {
      std::cerr << "ERROR: Failed to create GLFW Window" << std::endl;
      glfwTerminate();
      exit(EXIT_FAILURE);
    }
    // 'this' est associé à cette _window (pour les callback)
    glfwSetWindowUserPointer( _window, this);
    glfwMakeContextCurrent(_window);

  }
  // *************************************************** GLWindow::destruction
  ~GLWindow()
  {
    glfwDestroyWindow(_window);
    glfwTerminate();
  }
  // *********************************************************** GLWindow::run
  /** 
   * call render() every frame
   * stop with ESC */
  void run( void (*render)() )
  {
    // set key callback
    glfwSetKeyCallback(_window, key_callback);
    
    // render white window
    while (!glfwWindowShouldClose(_window)) {
      // update screen size
      glfwGetFramebufferSize(_window, &_screen_width, &_screen_height);
      
      // TODO Vérifier ce que fait glViewport
      glViewport(0, 0, _screen_width, _screen_height);
      /* Clear the background as black */
      glClearColor(0.f, 0.f, 0.f, 0.f);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

      render();

      glfwSwapBuffers(_window);
      glfwPollEvents();
    }
  }

  // ***************************************************************** attributs
  GLFWwindow* window() { return _window; };
  //private:
  GLFWwindow* _window;
  int _screen_width=800, _screen_height=600;

  // ****************************************************** GLWindow::callback
  /**
   * Callback qui gère les événements 'key'
   */
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    // ESC
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
  }
  /**
   * callback pour gérer les messages d'erreur de GLFW
   */
  static void error_callback(int error, const char* description)
  {
    std::cerr <<  description << std::endl;
  }
};


#endif // GL_WINDOW_HPP
