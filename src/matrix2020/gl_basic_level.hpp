/* -*- coding: utf-8 -*- */

#ifndef GL_BASIC_SCENE_HPP
#define GL_BASIC_SCENE_HPP

/** 
 * Implement a concept::GLScreen
 */

// OpenGL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// Model
#include <matrix2020/m_scene.hpp>
// Viewer
#include <matrix2020/gl_environment.hpp>
#include <matrix2020/gl_hacker.hpp>

using namespace matrix2020;

// ***************************************************************************
// ************************************************************** GLBasicLevel
// ***************************************************************************
class GLBasicLevel
{
public:
  // ************************************************** GLBasicLevel::creation
  GLBasicLevel( GLEngine& engine ) :
    _window( engine.window() ),
    _scene(nullptr)
  {
  }
  // ****************************************************** GLBasicLevel::init
  void init()
  {
    // Installe les callback pour les touches
    glfwSetWindowUserPointer( _window, this);
    glfwSetKeyCallback(_window, key_callback);

    // Init Scene
    _env.load_from_txt( "data/matrix00.txt" );
    _scene = new Scene( _env );
    _scene->init();

    // And graphic object
    _gl_env = new GLEnvironment( _env );
    _gl_hacker = new GLHacker( _scene->_hacker );
    
  }
  // **************************************************** GLBasicLevel::render
  void render()
  {
    // Transparence
    // Enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(_window)) {
       // update screen size
      glfwGetFramebufferSize(_window, &_screen_width, &_screen_height);
      
      glViewport(0, 0, _screen_width, _screen_height);
      /* Clear the background as black */
      glClearColor(0., 0., 0., 1.0);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

      // Render
      _gl_env->render();
      _gl_hacker->render();

      glfwSwapBuffers(_window);
      glfwPollEvents();
    }        
  }
  // *********************************************** GLBasicLevel::final_state
  bool final_state() const { return true; }
  // ************************************************* GLBasicLevel::attributs
  GLFWwindow* _window;
  int _screen_width, _screen_height;
  Environment _env;
  Scene *_scene;
  GLEnvironment *_gl_env;
  GLHacker *_gl_hacker;
  // ************************************************** GLBasicLevel::callback
  /**
   * Callback qui gère les événements 'key'
   */
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    // ESC => Quit"ACRUX HAMA X-Style Pad"
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
    // sinon callback de la classe
    else if( action == GLFW_PRESS ) {
      //std::cout << "key_callback = " << key << std::endl;
      ((GLBasicLevel *)glfwGetWindowUserPointer(window))->on_key_pressed( key );
    }
  }
  // ******************************************* GLBasicLevel::public_callback
  void on_key_pressed( int key ) 
  {
    //std::cout << "GLWindow::key_pressed key=" << key << std::endl;
    if( key == GLFW_KEY_UP) {
      _scene->on_key_up();
    }
    else if( key == GLFW_KEY_DOWN) {
      _scene->on_key_down();
    }
    else if( key == GLFW_KEY_LEFT) {
      _scene->on_key_left();
    }
    else if( key == GLFW_KEY_RIGHT) {
      _scene->on_key_right();
    }

  }
}; // GLBasicLevel

#endif // GL_BASIC_SCENE_HPP