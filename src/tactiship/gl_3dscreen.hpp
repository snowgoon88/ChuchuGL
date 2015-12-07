/* -*- coding: utf-8 -*- */

#ifndef GL_3DSCREEN_HPP
#define GL_3DSCREEN_HPP

/** 
 * TODO
 */

#include<gl_3dvect.hpp>

// OpenGL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// ******************************************************** GL3DScreen::Global
#define ZOOM_COEF  1.05f
#define ZOOM_MAX  10.0f
#define ZOOM_MIN   0.1f

typedef glm::vec2 Position;
enum class MouseAction {NOTHING,ZOOM};
// ***************************************************************************
// **************************************************************** GL3DScreen
// ***************************************************************************
class GL3DScreen
{
public:
  // **************************************************** GL3DScreen::creation
  GL3DScreen( GLEngine& engine ) :
    _window(engine.window()),
    _zoom(1.0), _start(0,0), _action(MouseAction::NOTHING),
    _finished(false)
  {
  };
  // ******************************************************** GL3DScreen::init
  /** Callback pour touches et souris */
  void init() 
  {
    // Installe les callback pour les touches
    glfwSetWindowUserPointer( _window, this);
    // glfwSetKeyCallback(_window, key_callback);
    glfwSetMouseButtonCallback( _window, mouse_button_callback );
    glfwSetCursorPosCallback( _window, mouse_move_callback );
    glfwSetScrollCallback( _window, scroll_callback);
  };
  // ********************************************************* GLScreen::render
  /**
   * render() est bloquant, ne rendant la main que quand le GLSCreen 
   * est terminé.
   * Pas de surprise : gère l'affichage en utilisant OpenGL.
   */
  void render () 
  {
    // Transparence
    // Enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    while (!glfwWindowShouldClose(_window) and not _finished) {
      // update screen size
      glfwGetFramebufferSize(_window, &_screen_width, &_screen_height);
      
      // TODO Vérifier ce que fait glViewport
      glViewport(0, 0, _screen_width, _screen_height);
      /* Clear the background as white */
      glClearColor(1., 1., 1., 1.0);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

      // Projection to screen
      glm::mat4 projection = glm::ortho( -3.f, 3.f, // left;right
					 -3.f, 3.f, // bottom,top
					 -3.f, 3.f // near far
					 );
      
      // Zoom
      glm::mat4 view = glm::scale( glm::mat4(1.0f),
				   glm::vec3( _zoom,
					      _zoom,
					      _zoom));
      glm::mat4 vp = projection * view;

      // _gl_vect.pre_render();
      _gl_vect.render( vp /*projection*/ );
      // _gl_vect.post_render();
      // Remove any programm so that glText can "work"
      glUseProgram(0);

      glfwSwapBuffers(_window);
      glfwPollEvents();

    }
  };
  // ************************************************** GL3DScreen::final_state
  bool final_state() const { return _finished; };
  // *************************************************** GL3DScreen::attributs
  GLFWwindow* window() { return _window; };
private:
  /** Ptr sur la Fenetre */
  GLFWwindow* _window;
  int _screen_width=800, _screen_height=600;
  float _zoom;
  Position _start;
  MouseAction _action;
  /** ready */
  bool _finished;
  /** Viewer */
  GL3DVect _gl_vect;
  // **************************************************** GL3DScreen::callback
  static void mouse_button_callback(GLFWwindow* window, int button,
				    int action, int mods)
  {
    // callback de la Classe
    ((GL3DScreen *)glfwGetWindowUserPointer(window))->on_mouse_button( button, action, mods );
  };
  static void mouse_move_callback( GLFWwindow* window, 
				   double xpos, double ypos) 
  {
    // callback de la Classe
    ((GL3DScreen *)glfwGetWindowUserPointer(window))->on_mouse_move( xpos, ypos);
  };
  static void scroll_callback(GLFWwindow* window,
			      double xoffset, double yoffset)
  {
    // callback de la Classe
    ((GL3DScreen *)glfwGetWindowUserPointer(window))->on_scroll( yoffset );
  };
public:
  // ********************************************* GL3DScreen::on_mouse_button
  void on_mouse_button( int button, int action, int mods ) 
  {
    double x, y;
    glfwGetCursorPos( _window, &x, &y);
    //std::cout <<"Mouse Button at (" << x <<  ", " << y << ")\n";
    
    if( action == GLFW_PRESS ) {
      if( button == GLFW_MOUSE_BUTTON_LEFT ) {
	// With SHIFT ??
	if( mods & GLFW_MOD_SHIFT ) {
	  //_scene->mouse_action_start ("move-resize",x,y);
	  std::cout << "move-resize at " << x << ", " << y  << std::endl;
	}
	else if( mods & GLFW_MOD_CONTROL ) {
	  std::cout << "zoom at " << x << ", " << y  << std::endl;
	}
	else {
	  //_scene->mouse_action_start ("rotate",x,y);
	  std::cout << "rotate at " << x << ", " << y  << std::endl;
	}
      }
      else if( button == GLFW_MOUSE_BUTTON_RIGHT ) {
	//_scene->mouse_action_start( "zoom", x, y);
	_start = Position(x,y);
	_action = MouseAction::ZOOM;
	std::cout << "btn right " << x << ", " << y  << std::endl;
      }
    }
    else if( action == GLFW_RELEASE ) {
      //_scene->mouse_action_end( x, y);
      _action = MouseAction::NOTHING;
      std::cout << "end_action " << x << ", " << y  << std::endl;
      std::cout << "           start= " << glm::to_string(_start) << std::endl;
    }
  };
  // *********************************************** GL3DScreen::on_mouse_move
  void on_mouse_move( double xpos, double ypos )
  {
    // En fonction des actions
    switch( _action ) {
    case MouseAction::ZOOM:
      _zoom = _zoom * (1.0 - (_start.y - ypos) / _screen_height);
      if( _zoom > ZOOM_MAX ) _zoom = ZOOM_MAX;
      else if( _zoom < ZOOM_MIN ) _zoom = ZOOM_MIN;
      _start = Position(xpos,ypos);
      break;
    case MouseAction::NOTHING:
    default:
      break;
    }
  };
  // *************************************************** GL3DScreen::on_scroll
  /** yoffset vaut +/- 1 */
  void on_scroll( double yoffset ) 
  {
    std::cout << "scroll_action yoffset=" << yoffset << std::endl;
    
    if( yoffset < 0 ) {
      _zoom *= ZOOM_COEF;
      if( _zoom > ZOOM_MAX ) _zoom = ZOOM_MAX;
    }
    else {
      _zoom /= ZOOM_COEF;
      if( _zoom < ZOOM_MIN ) _zoom = ZOOM_MIN;
    }
  };
};

#endif // GL_3DSCREEN_HPP
