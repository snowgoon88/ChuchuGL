/* -*- coding: utf-8 -*- */

#ifndef GL_WINDOW_HPP
#define GL_WINDOW_HPP

/** 
 * Une Fenetre GLFW où on dessine avec OpenGL.
 */

// OpenGL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <GLFW/glfw3.h>

// Model
#include <world.hpp>
// Viewer
#include <gl_world.hpp>

// ***************************************************************************
// ****************************************************************** GLWindow
// ***************************************************************************
class GLWindow
{
public:
  // ****************************************************************** creation
  /** Création avec titre et taille de fenetre.*/
  GLWindow(const std::string& title = "GLFW Window", int width=800, int height=600) :
    _screen_width(width), _screen_height(height),
    _gl_world(nullptr), _world(nullptr) 
  {
    std::cout << "Window creation" << std::endl;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    _window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!_window) {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(_window);
    glfwSetKeyCallback(_window, key_callback);
  };
  // *************************************************************** destruction
  virtual ~GLWindow()
  {
    glfwDestroyWindow(_window);
    glfwTerminate();
  };
  // ********************************************************************** init
  void init()
  {
    _world = std::unique_ptr<World>(new World());
    _world->init3x4();
    _gl_world = std::unique_ptr<GLWorld>(new GLWorld( *_world ));
  };
  // ******************************************************************** render
  void render ()
  {
    // Transparence
    // Enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Depth
    // glEnable( GL_DEPTH_TEST );

    // TODO cbk quand la fenètre est redimensionnée ??
    while (!glfwWindowShouldClose(_window)) {
      // float ratio;
      // int width, height;
      
      glfwGetFramebufferSize(_window, &_screen_width, &_screen_height);
      //float ratio = _screen_width / (float) _screen_height;
      
      glViewport(0, 0, _screen_width, _screen_height);
      glOrtho(-1.0f, 10.0f, -1.0f, 10.0f, 1.f, -1.f);
      

      /* Clear the background as white */
      glClearColor(1.0, 1.0, 1.0, 1.0);
      glClear(GL_COLOR_BUFFER_BIT); //|GL_DEPTH_BUFFER_BIT);

      // TODO Display cbk
      _gl_world->render();

      glfwSwapBuffers(_window);
      glfwPollEvents();
    }
  }
  // ***************************************************************** attributs
private:
  /** Ptr sur la Fenetre */
  GLFWwindow* _window;
  int _screen_width=800, _screen_height=600;
  //******************************************************************* callback
  /**
   * Callback qui gère les événements 'key'
   */
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
  }
  /**
   * Callback pour gérer les messages d'erreur de GLFW
   */
  static void error_callback(int error, const char* description)
  {
    std::cerr <<  description << std::endl;
  }
  // ********************************************************************* world
  std::unique_ptr<GLWorld> _gl_world;
  std::unique_ptr<World>   _world;
};

#endif // GL_WINDOW_HPP
