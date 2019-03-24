/* -*- coding: utf-8 -*- */

#ifndef GL_MOVEABLE_LEVEL_HPP
#define GL_MOVEABLE_LEVEL_HPP

/** 
 * Implements a concept::GLScreen that display and click-move Movables
 * using a GMMoveableManager
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

// Viewer
#include <matrix2020/gl_moveablemanager.hpp>

using namespace matrix2020;

// ***************************************************************************
// *********************************************************** GLMoveableLevel
// **************************************************************************
class GLMoveableLevel
{
public:
  // *********************************************** GLMoveableLevel::creation
  GLMoveableLevel( GLEngine& engine ) :
    _window( engine.window() )
  {
  }
  virtual ~GLMoveableLevel()
  {
    if (_gl_movmgr) {
      delete _gl_movmgr;
    }
  }
  // *************************************************** GLMoveableLevel::init
  void init()
  {
    // Installe les callback pour les touches
    glfwSetWindowUserPointer( _window, this);
    glfwSetKeyCallback(_window, key_callback);
    glfwSetMouseButtonCallback( _window, mouse_button_callback );
    glfwSetCursorPosCallback( _window, cursor_pos_callback );
    
    // Init Moveables
    _gl_movmgr = new GLMoveableManager();
    // first cards
    _cards.push_back( Moveable( "m00", {-1.0,1.33}, {1.0,-1.33},
                                {0.0, 209.f/210.f}, {156.f/464.f,0.0} ));
    _cards[0].pos( {0.5, 0.5} );
    _cards.push_back( Moveable( "m01", {-1.0,1.33}, {1.0,-1.33},
                                {156.f/464.f, 209.f/210.f}, {308.f/464.f,0.0} ));
    _cards[1].pos( {2.5, 0.5} );
    _cards.push_back( Moveable( "m02", {-1.0,1.33}, {1.0,-1.33},
                                {310.f/464.f, 209.f/210.f}, {462.f/464.f,0.0} ));
    _cards[2].pos( {2.5, 2.5} );
    
    // add to Manager
    for (auto itm = _cards.begin(); itm != _cards.end(); ++itm) {
      _gl_movmgr->add_moveable( &(*itm) );
    }
    // set second as current
    //_gl_movmgr->_current_moveable = &(_cards[1]);
    
    // then Sinks
    _sinks.push_back( Sink( "s00", {-0.5,3.0}, {-1.0,1.33}, {1.0,-1.33}  ));
    _sinks.push_back( Sink( "s01", {2.0,3.0}, {-1.0,1.33}, {1.0,-1.33}  ));
    for (auto its = _sinks.begin(); its != _sinks.end(); ++its) {
      _gl_movmgr->add_sink( &(*its) );
    }
    _gl_movmgr->_current_sink = &(_sinks[0]);
    
    //_gl_movmgr->update_vbo();
    std::cout << "__GL_MoveableManager" << std::endl;
    std::cout << _gl_movmgr->str_dump() << std::endl;
  }
  // ************************************************* GLMoveableLevel::render
  void render()
  {
    // Camera mode : static
    _proj = glm::mat4( 1.0f );
    _proj = glm::ortho( -3.f, 5.f, // left;right
                        -3.f, 5.f, // bottom,top
                        -1.f, 100.f // near far
                        );
    _view = glm::mat4( 1.0f );
    _view = glm::translate(_view, glm::vec3(0.0f, 0.0f, -10.0f));
    _projview = _proj * _view;

    while (!glfwWindowShouldClose(_window)) {
       // update screen size
      glfwGetFramebufferSize(_window, &_screen_width, &_screen_height);
      // text_shaders assume a screen -1,1 x -1,1
      float sx = 2.f / _screen_width;
      float sy = 2.f / _screen_height;
      glViewport(0, 0, _screen_width, _screen_height);
      /* Clear the background as black */
      glClearColor(0., 0., 0., 1.0);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

      
      // Update model
      _gl_movmgr->update_vbo();
      // Render
      //std::cout << "__RENDER BASIC LEVEL" << std::endl;
      _gl_movmgr->render( _projview );

      glfwSwapBuffers(_window);
      glfwPollEvents();
    }        
  }
  // ******************************************** GLMoveableLevel::final_state
  bool final_state() const { return true; }
  // ********************************************** GLMoveableLevel::attributs
  GLFWwindow* _window;
  int _screen_width, _screen_height;
  glm::mat4 _proj, _view, _projview;

  std::vector<Moveable> _cards;
  std::vector<Sink> _sinks;
  GLMoveableManager* _gl_movmgr;

  // ************************************************** GLBasicLevel::callback
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
      ((GLMoveableLevel *)glfwGetWindowUserPointer(window))->on_key_pressed( key );
    }
  }
  // ******************************************* GLBasicLevel::public_callback
  void on_key_pressed( int key )
  {
    //std::cout << "GLWindow::key_pressed key=" << key << std::endl;
  }
  // ***************************************************** GLBasicLevel::mouse
  static void mouse_button_callback(GLFWwindow* window, int button,
				    int action, int mods)
  {
    // callback from Classe
    ((GLMoveableLevel *)glfwGetWindowUserPointer(window))->on_mouse_button( button, action, mods );
  }
  void on_mouse_button( int button, int action, int mods ) 
  {
    double x, y;
    glfwGetCursorPos( _window, &x, &y);
    std::cout <<"Mouse Button at (" << x <<  ", " << y << ")\n";

    if( action == GLFW_PRESS ) {
      if( button == GLFW_MOUSE_BUTTON_LEFT ) {
        // Infer the model that has been clicked.
        auto pt = glm::vec4{ (float)x, (float)y, 0.f, 1.f };
        if( get_mouse_horizontal( pt )){
          std::cout << "  cliked press on " << glm::to_string(pt) << std::endl;

          _gl_movmgr->on_mouse_press( {pt[0], pt[1]} );
        }
      }
    }
    else if (action == GLFW_RELEASE ) {
      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        // Infer the model that has been clicked.
        auto pt = glm::vec4{ (float)x, (float)y, 0.f, 1.f };
        if( get_mouse_horizontal( pt )){
          std::cout << "  cliked release on " << glm::to_string(pt) << std::endl;
        _gl_movmgr->on_mouse_release( {pt[0], pt[1]} );
        }
      }
    }
  }
   /**
   * Compute the pt clicked by the mouse which is on the Oxy plane in World 
   * coordinate
   *
   * Param: pt_mouse = (x,y,*,*) in Screen Coordinate
   * Return:
   *  - pt_mouse = (x,y,z,1) in World Coordinate
   *  - true if intersection, false otherwise.
   */
  bool get_mouse_horizontal( glm::vec4& pt_mouse ) const
  {
    // Normalised in [-1;1],[-1;1],z,w
    pt_mouse.x = 2.f * pt_mouse.x / _screen_width - 1.f;
    pt_mouse.y = -2.f * pt_mouse.y / _screen_height + 1.f;
    pt_mouse.z = 0.f;
    pt_mouse.w = 1.f;
    // Camera coordinate, unprojected
    pt_mouse = glm::inverse( _proj ) * pt_mouse;
    // World coordinate, unviewed
    pt_mouse = glm::inverse( _view ) * pt_mouse;
    //std::cout << "  computed at " << glm::to_string(pt_mouse) << std::endl;
    
     // As we use orthogonal projection, need only the World coordinate of 0,0,1
    auto vec_w = glm::vec4(0.f,0.f, 1.f, 0.f);
    vec_w = glm::inverse( _view ) * vec_w;
    
    // Then compute the projection of pt_mouse on 0xy
    if (fabs(vec_w.z) > 0.001) {
      auto t0 = pt_mouse.z / vec_w.z;
      pt_mouse = pt_mouse - vec_w * t0;
      return true;
    }
    return false;
  }
  // ************************************ GLMoveableLevel::mouse move callback
  static void cursor_pos_callback(GLFWwindow* window, double posx, double posy )
  {
    // callback from Classe
    ((GLMoveableLevel *)glfwGetWindowUserPointer(window))->on_mouse_move( posx, posy );
  }
  void on_mouse_move( double posx, double posy )
  {
    // only if MoveableManger is grabbing
    if ( _gl_movmgr->is_grabbing() ) {
      auto pt = glm::vec4{ (float)posx, (float)posy, 0.f, 1.f };
      //std::cout << "  moving at " <<  glm::to_string(pt) << std::endl;
      if( get_mouse_horizontal( pt )){
        _gl_movmgr->on_move( {pt[0], pt[1]} );
      }
    }
  }
}; // class GLMoveableLevel

#endif // GL_MOVEABLE_LEVEL_HPP
