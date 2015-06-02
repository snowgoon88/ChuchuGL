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

#include <fstream>
#include <chrono>                      // std::chrono
#include <thread>                      // std::thread

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
    _gl_world(nullptr), _world(nullptr), _is_running(false)
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
    // 'this' est associé à cette _window (pour les callback)
    glfwSetWindowUserPointer( _window, this);
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
    _world = std::unique_ptr<World>(new World( "data/world_6x5.json"));
    //_world->init3x4();
    // Open file
    // std::ifstream myfile( "../data/world_6x5.json" );
    // _world->read_json( myfile );
    // myfile.close();
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
    glEnable( GL_DEPTH_TEST );

    // TODO cbk quand la fenètre est redimensionnée ??

    
    while (!glfwWindowShouldClose(_window)) {
      // clock
      auto start_proc = std::chrono::steady_clock::now();
      // world update
      if( _is_running ) {
	_world->update( 0.020 );
      }
      // float ratio;
      // int width, height;
      
      glfwGetFramebufferSize(_window, &_screen_width, &_screen_height);
      //float ratio = _screen_width / (float) _screen_height;
      
      glViewport(0, 0, _screen_width, _screen_height);
      glOrtho(-1.0f, 10.0f, -1.0f, 10.0f, 1.f, -1.f);
      

      /* Clear the background as white */
      glClearColor(1.0, 1.0, 1.0, 1.0);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

      // Display cbk
      _gl_world->render();

      glfwSwapBuffers(_window);
      glfwPollEvents();

      // clock
      auto end_proc = std::chrono::steady_clock::now();
      // wait period 
      std::chrono::duration<double> elapsed_seconds = end_proc - start_proc;
      std::this_thread::sleep_for(std::chrono::milliseconds(20)
				  - elapsed_seconds );
      //std::cout << "TIME = " <<  elapsed_seconds.count() << std::endl;
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
    // ESC => Quit
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
    // sinon callback de la classe
    else if( action == GLFW_PRESS ) {
      //std::cout << "key_callback = " << key << std::endl;
      ((GLWindow *)glfwGetWindowUserPointer(window))->on_key_pressed( key );
    }
  }
  // ********************************************************* public_callback
  void on_key_pressed( int key ) 
  {
    //std::cout << "on_key_presssed = " << key << std::endl;
    if( key == GLFW_KEY_R) {
      // toggle _is_running
      _is_running = !_is_running;
    }
    else if( key == GLFW_KEY_D) {
      // display les chuchu
      std::cout << _world->str_display() << std::endl;
    } 
  };
  /**
   * Callback pour gérer les messages d'erreur de GLFW
   */
  static void error_callback(int error, const char* description)
  {
    std::cerr <<  description << std::endl;
  }
  // ******************************************************************* world
  std::unique_ptr<GLWorld> _gl_world;
  std::unique_ptr<World>   _world;
  // ************************************************************** simulation
  bool _is_running;
};

#endif // GL_WINDOW_HPP
