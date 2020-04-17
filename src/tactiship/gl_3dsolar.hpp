/* -*- coding: utf-8 -*- */

#ifndef GL3DSOLAR_HPP
#define GL3DSOLAR_HPP

/** 
 * Display a Icosphere.
 */
#include <gl_3dengine.hpp>
#include <gl_3dframe.hpp>
#include <gl_3dicosphere.hpp>

// OpenGL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <GLFW/glfw3.h>

#include <visugl/trackball.hpp>

class GL3DSolar
{
public:
  // ***************************************************** GL3DSolar::creation
  GL3DSolar( GL3DEnginePtr engine ) :
    _engine(engine),
    _window(engine->window()), _trackball(),
    _finished(false),
    _viewer_frame(engine),
    _icosphere(engine)                                        // One Icosphere
  {
  }
  // ********************************************************* GL3DSolar::init
  // Callback for keys and mouse -> concept::Screen
  void init()
  {
    // Installe les callback pour les touches
    glfwSetWindowUserPointer( _window, this);
    glfwSetKeyCallback(_window, key_callback);
    glfwSetMouseButtonCallback( _window, mouse_button_callback );
    glfwSetCursorPosCallback( _window, mouse_move_callback );
    glfwSetScrollCallback( _window, scroll_callback);
    std::cout << "__INIT" << std::endl;
  }

  // ******************************************************* GL3DSolar::render
  /** -> concept::Screen
   * render() is blocking, returns when GLScreen has ended.
   */
  void render ()
  {
    // Transparence -> TEXT
    // Enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    while (!glfwWindowShouldClose(_window) and not _finished) {
      // update screen size
      glfwGetFramebufferSize(_window, &_win_width, &_win_height);

      _engine->gl_text().set_scale( (10.f)/(float)_win_width,
                                    (10.f)/(float)_win_height );
          
      // Viewport to entire window
      glViewport(0, 0, _win_width, _win_height);

       /* Clear the background as black */
      glClearColor(0., 0., 0., 1.0);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

      // Projection to screen
      glm::mat4 projection = glm::ortho( -2.f, 2.f, // left;right
					 -2.f, 2.f, // bottom,top
					 -100.f, 100.f // near far
					 );
      glm::mat4 mvp = projection * _trackball.get_transform();
      // Invert Z axis as OpenGL is left-handed
      glm::mat4 z_inverter = glm::scale( glm::mat4(1.0f),
        			   glm::vec3( 1, 1, -1 ));
      mvp = mvp * z_inverter;
      
      // rotation of icosphere
      auto rot = glm::quat(glm::vec3(0,0,0));
      //rot = glm::rotate( rot, (float) M_PI/4.f, {1.f,0.f,0.f} );
      _icosphere.render( mvp, {0.5f, 0.5f, 0.f}, rot );
      _viewer_frame.render( mvp /*projection*/ );
      
      // Remove any programm so that glText can "work"
      glUseProgram(0);
      glfwSwapBuffers(_window);
      glfwPollEvents();
    }
    _finished = true;
  }
  // ************************************************** GL3DSolar::final_state
  // -> concept::GLScreen
  bool final_state() const { return _finished; };
  // **************************************************** GL3DSolar::attributs
private:
  GL3DEnginePtr _engine;
  /** Ptr sur la Fenetre */
  GLFWwindow* _window;
  int _win_width=800, _win_height=600;
  /** Trackball */
  visugl::TrackBall _trackball;
  
  /** ready */
  bool _finished;
  /** Viewers */
  GL3DFrame _viewer_frame;
  GL3DIcosphere _icosphere;
  

  // *************************************************************************
  // ************************************************ Callback for interaction
  // *************************************************************************
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    // ESC
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
  }
  static void mouse_button_callback(GLFWwindow* window, int button,
				    int action, int mods)
  {
    // callback de la Classe
    ((GL3DSolar *)glfwGetWindowUserPointer(window))->on_mouse_button( button, action, mods );
  }
  static void mouse_move_callback( GLFWwindow* window, 
				   double xpos, double ypos) 
  {
    // callback de la Classe
    ((GL3DSolar *)glfwGetWindowUserPointer(window))->on_mouse_move( xpos, ypos);
  };
  static void scroll_callback(GLFWwindow* window,
			      double xoffset, double yoffset)
  {
    // callback de la Classe
    ((GL3DSolar *)glfwGetWindowUserPointer(window))->on_mouse_scroll( xoffset, yoffset );
  };
  // *********************************************** GL3DSolar::mouse_callback
  virtual void on_mouse_button( int button, int action, int mods )
  {
    double x, y;
    glfwGetCursorPos( _window, &x, &y);
    //std::cout <<"Figure3D::Mouse Button at (" << x <<  ", " << y << ")\n";

    // clear_text();
    // std::stringstream msg;
    // msg << "Mouse : (" << x <<  ", " << y << ")";
    // add_text( msg.str(), 0.02, 0.90 );

    _trackball.on_mouse_btn( button, action, mods,
                             x/(_win_width), y/(_win_height));
  }
  virtual void on_mouse_move( double xpos, double ypos )
  {
    double x, y;
    glfwGetCursorPos( _window, &x, &y);
    //std::cout <<"Figure3D::Mouse move at (" << x <<  ", " << y << ")\n";

    // clear_text();
    // std::stringstream msg;
    // msg << "Mouse move : (" << x <<  ", " << y << ")";
    // add_text( msg.str(), 0.02, 0.90 );

    _trackball.on_mouse_move( x/(_win_width), y/(_win_height) );
  }
  virtual void on_mouse_scroll( double xoffset, double yoffset )
  {
    _trackball.on_mouse_scroll( xoffset, yoffset );
  }
  
}; // GL3DSolar

#endif // GL3DSOLAR_HPP
