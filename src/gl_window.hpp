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
    _screen_width(width), _screen_height(height),
    _key_debug(false)
  {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
      std::cerr << "ERROR: Failed to init GLFW" << std::endl;
      exit(EXIT_FAILURE);
    }
    // opengl 3.3 core
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    

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
  // ********************************************************** GLWindow::init
  /**
   * give some info
   */
  void init()
  {
    double gl_data[4];
    glGetDoublev( GL_DEPTH_RANGE, gl_data );
    std::cout << "GL_DEPTH_RANGE: " << gl_data[0] << ", " << gl_data[1];
    std::cout << std::endl;

    // set key callback
    glfwSetKeyCallback(_window, key_callback);
    glfwSetCharCallback(_window, character_callback);

  }
  // *********************************************************** GLWindow::run
  /** 
   * call render() every frame
   * stop with ESC */
  void run( void (*render)() )
  {
    // render white window
    while (!glfwWindowShouldClose(_window)) {
      // update screen size
      glfwGetFramebufferSize(_window, &_screen_width, &_screen_height);
      
      // TODO Vérifier ce que fait glViewport
      glViewport(0, 0, _screen_width, _screen_height);
      /* Clear the background as black */
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
  bool _key_debug;
  
  // ****************************************************** GLWindow::callback
  /**
   * Callback qui gère les événements 'key'
   */
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    // ESC
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);

    
    if (((GLWindow *)glfwGetWindowUserPointer(window))->_key_debug) {
      std::cout << "key=" << key << "(" << std::hex << key << std::dec << ")";
      std::cout << "\tscan=" << scancode << "(" << std::hex << scancode << std::dec << ")";
      std::cout << "\tact=" << action << "\tmods=" << mods << std::endl;
    }
  }
  /**
   * callback for TextInput
   */
  static void character_callback(GLFWwindow* window, unsigned int codepoint)
  {
    if (((GLWindow *)glfwGetWindowUserPointer(window))->_key_debug) {
      std::cout << "Char=" << codepoint << std::endl;
      std::wstring uc = std::to_wstring( codepoint );
      std::wcout << "uChar=" << codepoint << "\tuc= " << uc << std::endl;
    }
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
