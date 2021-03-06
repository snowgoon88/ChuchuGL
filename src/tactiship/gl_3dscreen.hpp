/* -*- coding: utf-8 -*- */

#ifndef GL_3DSCREEN_HPP
#define GL_3DSCREEN_HPP

/** 
 * TODO
 */

#include <trackball.h>
#include <gl_3dengine.hpp>
#include <gl_3dframe.hpp>
#include <gl_3dship.hpp>
#include <trajectory.hpp>

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

typedef glm::vec2  ScreenPos;
enum class         MouseAction {NOTHING,ZOOM,ROTATE,MOVE};
typedef float      Quaternion[4];          // cf trackball.h
typedef float      RotMatrix[16];          // cf trackball.h
// ***************************************************************************
// **************************************************************** GL3DScreen
// ***************************************************************************
class GL3DScreen
{
public:
  // **************************************************** GL3DScreen::creation
  GL3DScreen( GL3DEnginePtr engine, const GL3DTrajectory& traj_viewer ) :
    _engine(engine),
    _window(engine->window()),
    _zoom(1.0), _start(0,0), _pos{0,0}, _orient{0,0,0,1}, 
    _action(MouseAction::NOTHING),
    _finished(false),
    _viewer_frame(engine), _viewer_ship(engine),
    _viewer_traj(traj_viewer)
  {
  };
  // ******************************************************** GL3DScreen::init
  /** Callback pour touches et souris */
  void init() 
  {
    // Installe les callback pour les touches
    glfwSetWindowUserPointer( _window, this);
    glfwSetKeyCallback(_window, key_callback);
    glfwSetMouseButtonCallback( _window, mouse_button_callback );
    glfwSetCursorPosCallback( _window, mouse_move_callback );
    glfwSetScrollCallback( _window, scroll_callback);
  };
  // ******************************************************** GLScreen::render
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

      _engine->gl_text().set_scale( (10.f)/(float)_screen_width,
                                    (10.f)/(float)_screen_height );
          
      // TODO Vérifier ce que fait glViewport
      glViewport(0, 0, _screen_width, _screen_height);
      /* Clear the background as white */
      glClearColor(1., 1., 1., 1.0);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

      // Projection to screen
      glm::mat4 projection = glm::ortho( -10.f, 10.f, // left;right
					 -10.f, 10.f, // bottom,top
					 -100.f, 100.f // near far
					 );
      
      // Zoom
      glm::mat4 zoom = glm::scale( glm::mat4(1.0f),
				   glm::vec3( _zoom,
					      _zoom,
					      _zoom));
      // Rotation
      RotMatrix view_rotation;
      build_rotmatrix( view_rotation, _orient );
      glm::mat4 rotation = glm::make_mat4x4( view_rotation );

      // Translation
      glm::mat4 translation = glm::translate(  glm::mat4(1.0f),
					       glm::vec3( _pos.x,
							  _pos.y,
							  0.f));
      // Projection-View
      glm::mat4 vp = projection * zoom * translation * rotation;

      // Pré render => non
      _viewer_ship.render( vp );
      _viewer_ship.render( vp, {1.f, 0.f, 2.f},
	  		   // quaternion rotation from pitch, yaw, roll
	  		   // roll (around 0x), pitch (0y), yaw (0z)
	  		   glm::quat(glm::vec3{-M_PI/6.0,0,M_PI/4.}) );
      _viewer_ship.render( vp, {1.f, 1.f, -2.f},
	  		   // quaternion rotation from pitch, yaw, roll
	  		   // roll (around 0x), pitch (0y), yaw (0z)
	  		   glm::quat(glm::vec3{0,0,-M_PI/3.0}),
	  		   {2.f, 0.5f, 0.2f} );
      _viewer_traj.render( vp );
      _viewer_frame.render( vp /*projection*/ );
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
  GL3DEnginePtr _engine;
  /** Ptr sur la Fenetre */
  GLFWwindow* _window;
  int _screen_width=800, _screen_height=600;
  float _zoom;
  ScreenPos _start;
  ScreenPos _pos;
  Quaternion _orient;
  MouseAction _action;
  /** ready */
  bool _finished;
  /** Viewer */
  GL3DFrame _viewer_frame;
  GL3DShip  _viewer_ship;
  const GL3DTrajectory& _viewer_traj;
  // **************************************************** GL3DScreen::callback
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    // ESC
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
  };

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
	  _start = ScreenPos(x,y);
	  _action = MouseAction::MOVE;
	}
	else if( mods & GLFW_MOD_CONTROL ) {
	  std::cout << "zoom at " << x << ", " << y  << std::endl;
	}
	else {
	  //_scene->mouse_action_start ("rotate",x,y);
	  _start = ScreenPos(x,y);
	  _action = MouseAction::ROTATE;
	  std::cout << "rotate at " << x << ", " << y  << std::endl;
	}
      }
      else if( button == GLFW_MOUSE_BUTTON_RIGHT ) {
	//_scene->mouse_action_start( "zoom", x, y);
	_start = ScreenPos(x,y);
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
      _start = ScreenPos(xpos,ypos);
      break;
    case MouseAction::ROTATE:
      Quaternion d_quat;
      trackball( d_quat,
		 (2.0 * _start.x - _screen_width) / _screen_width,
		 (_screen_height - 2.0 * _start.y) / _screen_height,
		 (2.0 * xpos - _screen_width) / _screen_width,
		 (_screen_height - 2.0 * ypos) / _screen_height);
      add_quats( d_quat, _orient, _orient );
      _start = ScreenPos(xpos,ypos);
      break;
    case MouseAction::MOVE:
      _pos += ScreenPos(2*(xpos-_start.x)/_screen_width,
		       -2*(ypos-_start.y)/_screen_height);
      _start = ScreenPos(xpos,ypos);
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
