/* -*- coding: utf-8 -*- */

#ifndef GL_3DAPP_HPP
#define GL_3DAPP_HPP

/** 
 * A GLFW Window that can 'run' a 'xxxScreen'
 *
 * The 'screen' is added as an '_event_listener' for key, mouse_button,
 * mouse_move and mouse_scroll.
 * The 'screen' is rendered.
 *
 * ESC closes the application.
 *
 * @see 'solar_screen.hpp' for working example.
 */
#include <iostream>                       // std::cout
#include <string>
#include <list>

// OpenGL NEED to be included before GLFW otherwise an 'older' version
// gl.h is included (strange)
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <GLFW/glfw3.h>
//#include <visugl/trackball.hpp>
#include <3d/gl_3dscreen.hpp>

// ***************************************************************************
// ******************************************************************* GL3DApp
// ***************************************************************************
class GL3DApp
{
  using EventListenersPtr = GL3DScreen*;
public:
  // ******************************************************* GL3DApp::creation
  GL3DApp(const std::string& title = "GL Application",
          const int width=800, const int height=800,
          const int posx=-1, const int posy = -1,
          const bool fullsize=false) :
    _window(nullptr),
    _win_width(width), _win_height(height)
    
  {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
      std::cerr << "ERROR: Failed to init GLFW" << std::endl;
      exit(EXIT_FAILURE);
    }

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
    // for callback, associate to 'this'
    glfwSetWindowUserPointer( _window, this);
    glfwMakeContextCurrent(_window);
    glfwSetKeyCallback( _window, key_callback);
    glfwSetWindowSizeCallback( _window, resize_callback );
    glfwSetMouseButtonCallback( _window, mouse_button_callback );
    glfwSetCursorPosCallback( _window, mouse_move_callback );
    glfwSetScrollCallback( _window, scroll_callback);
  }
  // ***************************************************** GL3DApp::destructor
  virtual ~GL3DApp()
  {
    if (_window) {
      glfwSetWindowShouldClose(_window, GL_TRUE);
      glfwDestroyWindow( _window);
      _window = nullptr;
    }
  }
  // ************************************************************ GL3DApp::run
  void run( GL3DScreen* screen )
  {
    // Add it to EventListener
    _event_listeners.push_back( screen );

    while (!glfwWindowShouldClose(_window)) {
      screen->render( _win_width, _win_height );
      
      glfwSwapBuffers(_window);
      glfwPollEvents();
    }
  }
protected:
  GLFWwindow* _window;
  int _win_width, _win_height;
  std::list<EventListenersPtr> _event_listeners;

  // ******************************************************** GL3Dapp::statics
  /** Key Callbacks */
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    // call Class method
    ((GL3DApp *)glfwGetWindowUserPointer(window))->on_key( key, scancode, action, mods );
  }
  virtual void on_key( int key, int scancode, int action, int mods)
  {
    // ESC
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(_window, GL_TRUE);
    
    // call registered callbacks
    for( auto& listener: _event_listeners) {
      listener->key_callback( key, scancode, action, mods );
    }
  }
  /** Resize Callbacks */
  static void resize_callback( GLFWwindow* window, int width, int height )
  {
    // call Class method
    ((GL3DApp *)glfwGetWindowUserPointer(window))->on_resized();
  }
  virtual void on_resized()
  {
    glfwGetFramebufferSize( _window, &_win_width, &_win_height);
  }
  /** Mouse Callbacks */
  static void mouse_button_callback(GLFWwindow* window, int button,
				    int action, int mods)
  {
     // call Class method
    ((GL3DApp *)glfwGetWindowUserPointer(window))->on_mouse_button( button, action, mods );
  }
  virtual void on_mouse_button( int button, int action, int mods )
  {
    double x, y;
    glfwGetCursorPos( _window, &x, &y);
    // _trackball.on_mouse_btn( button, action, mods,
    //                        x/(_win_width), y/(_win_height));

    // call registered callbacks
    for( auto& listener: _event_listeners) {
      listener->mouse_button_callback( button, action, mods,
                                       x, y, _win_width, _win_height );
    }
  }
  static void mouse_move_callback( GLFWwindow* window, 
				   double xpos, double ypos)
  {
    // call Class method
    ((GL3DApp *)glfwGetWindowUserPointer(window))->on_mouse_move( xpos, ypos );
  }
  virtual void on_mouse_move( double xpos, double ypos )
  {
    // call registered callbacks
    for( auto& listener: _event_listeners) {
      listener->mouse_move_callback( xpos, ypos, _win_width, _win_height );
    }
  }
  static void scroll_callback(GLFWwindow* window,
			      double xoffset, double yoffset)
  {
    // call Class method
    ((GL3DApp *)glfwGetWindowUserPointer(window))->on_scroll( xoffset, yoffset );
  }
  virtual void on_scroll( double xoffset, double yoffset )
  {
    // call registered callbacks
    for( auto& listener: _event_listeners) {
      listener->mouse_scroll_callback( xoffset, yoffset );
    }
  }
  /** GLFW error messages */
  static void error_callback(int error, const char* description)
  {
    std::cerr <<  description << std::endl;
  }
}; // GL3DApp


#endif // GL_3DAPP_HPP
