/* -*- coding: utf-8 -*- */

#ifndef GL_BOIDS_SCREEN_HPP
#define GL_BOIDS_SCREEN_HPP

/** 
 * A 2.5D env with :
 *  - frame reference and grid
 *  - list of orented tokens
 *
 * TODO : remove trackball and set proper 2.5D controler
 *
 * Implements aglconcept::GLScreen (see TODO)
 */

#include <trackball.h>

#include <tactiship/gl_3dengine.hpp>
#include <tactiship/gl_3dframe.hpp>
#include <tactiship/gl_3dgrid.hpp>
#include <tactiship/gl_3drect.hpp>
#include <boid/gl_token.hpp>
#include <list>

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
#include <glm/gtc/quaternion.hpp>     // definition
#include <glm/gtx/quaternion.hpp>     // rotation of vec3

// Physics
#include <physics/engine.hpp>
#include <boid_body.hpp>

#include <sstream>
#include <iomanip> // needed to use manipulators with parameters (precision, width)
#include <chrono>                      // std::chrono
#include <thread>                      // std::thread

// ***************************************************** GLBoidsScreen::Global
#define ZOOM_COEF  1.05f
#define ZOOM_MAX  10.0f
#define ZOOM_MIN   0.1f

//typedef glm::vec2  ScreenPos;
using ScreenPos = glm::vec2;
enum class         MouseAction {NOTHING,ZOOM,ROTATE,MOVE};
using Quaternion = float[4];          // cf trackball.h
using RotMatrix = float[16];          // cf trackball.h

// ***************************************************************************
// ************************************************************* GLBoidsScreen
// ***************************************************************************
class GLBoidsScreen
{
  using GL3DEnginePtr = std::shared_ptr<GL3DEngine>;
public:
  // ************************************************* GLBoidsScreen::creation
  GLBoidsScreen( GL3DEnginePtr engine,
                 float xmin=-10.f, float xmax=10.f, float xgap=1.f,
                 float ymin=-10.f, float ymax=10.f, float ygap=1.f ) :
    _window(engine->window()),
    _zoom(1.0), _start(0,0), _pos{0,0}, _orient{0,0,0,1}, 
    _action(MouseAction::NOTHING),
    _finished(false),
    _viewer_frame( engine ),
	_viewer_grid( engine, xmin, xmax, xgap, ymin, ymax, ygap ),
	_viewer_token( engine, 32 ),
	_gl_text( engine->gl_text() ),
    _gui_rect( engine ),
    _physics_running(false), _physics_eng(nullptr), _physics_time(0.0),
    _frame_time_cur(0.0), _frame_time_mean(0.0),
    _phys_time_cur(0.0), _phys_time_mean(0.0)
  {
	_gui_rect.set_color( {0.8f, 0.8f, 0.8f}, 0.2f );

    physics_init();
    
    // // Create some Tokens to display
    // // 1, 0 + 0rad
    // glm::vec3 ori {1, 0, 0};
    // float orient {M_PI * 0.f};
    // glm::vec3 tok_color {1.f, 0.f, 0.f};
    // glm::vec3 arr_color {0.2f, 0.2f, 0.2f};
    // _token_l.push_back( GLToken::TToken{ ori, orient, tok_color, arr_color } );
    // ori = glm::vec3{1.5, 2, 0};
    // orient = M_PI / 4.f;
    // tok_color = glm::vec3{0.f, 1.f, 0.f};
    // arr_color = glm::vec3{0.2f, 0.2f, 0.9f};
    // _token_l.push_back( GLToken::TToken{ ori, orient, tok_color, arr_color } );
    std::cout << "__BOIDS : " << _token_l.size() << std::endl;
  }
  GLToken::TToken token_from_boid( BoidBodyPtr boid )
  {
    return GLToken::TToken {
      boid->_pos,
      glm::angle( boid->_rot ),
      {1.f, 0.f, 0.f},
      {0.2f, 0.2f, 0.2f}};
  }
  // *********************************************** GLBoidsScreen::destructor
  virtual ~GLBoidsScreen()
  {
    if( _physics_eng ) delete _physics_eng;
  }
  // ***************************************************** GLBoidsscreen::init
  /** Callback pour touches et souris */
  void init() 
  {
    // Installe les callback pour les touches
    glfwSetWindowUserPointer( _window, this);
    glfwSetKeyCallback(_window, key_callback);
    glfwSetMouseButtonCallback( _window, mouse_button_callback );
    glfwSetCursorPosCallback( _window, mouse_move_callback );
    glfwSetScrollCallback( _window, scroll_callback);
  }
  // ********************************************* GLBoidsScreen::physics_init
  void physics_init()
  {
    _physics_eng = new physics::Engine( false /* no gravity */ );

    _boid_list.clear();
    auto pt = BoidBodyPtr( new BoidBody() );
    _physics_eng->_bodies.push_back( pt );
    _boid_list.push_back( pt );

    pt = BoidBodyPtr( new BoidBody() );
    _physics_eng->_bodies.push_back( pt );
    _boid_list.push_back( pt );

    physics_reset();
  }
  void physics_reset()
  {
    // TODO: not clean !!
    auto it = _boid_list.begin(); 
    // First boid
    auto pt = *it++;
    pt->_pos = physics::TVec3{1,0,0};
    pt->_rot = glm::angleAxis( 0.f, physics::TVec3(0,0,1));
    pt->_vel = physics::TVec3{1,0,0};
    // second
    pt = *it++;
    pt->_pos = physics::TVec3{1.5,2,0};
    pt->_rot = glm::angleAxis( (float) M_PI, physics::TVec3(0,0,1));
    pt->_vel = physics::TVec3{0,0.5,0};
  }
  // *************************************************** GLBoidsScreen::render
  /**
   * render() blocks, returns only when GLScreen is finished.
   */
  void render ()
  {
    // Transparence
    // Enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    auto frame_time = std::chrono::steady_clock::now();
    std::stringstream fps_ss;
    fps_ss << "FPS c/M : ";
    fps_ss << _frame_time_cur.count() << "/";
    fps_ss << _frame_time_mean.count();
    fps_ss << " PHYS c/m : ";
    fps_ss << _phys_time_cur.count() << "/";
    fps_ss << _phys_time_mean.count();

    // Physics status
    std::stringstream phy_ss;

    while (!glfwWindowShouldClose(_window) and not _finished) {
      // clock
      auto start_proc = std::chrono::steady_clock::now();
      // Physics
      phy_ss.str("");
      phy_ss << "(R)un, r(E)set : ";
      if( _physics_running ) {
        phy_ss << "RUN";
        _physics_eng->update( 0.02 );
        _physics_time += 0.02;
        // do things
      }
      else {
        phy_ss << "STOP";
      }
      phy_ss << " t=";
      phy_ss << std::fixed << std::setprecision(2);
      phy_ss << _physics_time;
      auto end_physics = std::chrono::steady_clock::now();
      
      // update screen size
      glfwGetFramebufferSize(_window, &_screen_width, &_screen_height);

      _gl_text.set_scale( (20.f)/(float)_screen_width,
                          (20.f)/(float)_screen_height );

      
      // TODO Vérifier ce que fait glViewport
      glViewport(0, 0, _screen_width, _screen_height);
      /* Clear the background as white */
      glClearColor(1., 1., 1., 1.0);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    
      // Camera mode
      glm::mat4 vp;
      glm::mat4 zoom, rotation, translation;
      // Trackball view -----------------------------------------------------
      // using a ortho view and using the mouse buttons as trackball.
      // Projection to screen
      _proj = glm::ortho( -10.f, 10.f, // left;right
                         -10.f, 10.f, // bottom,top
                         -100.f, 100.f // near far
                         );
      // Zoom
      zoom = glm::scale( glm::mat4(1.0f),
                         glm::vec3( _zoom,
                                    _zoom,
                                    _zoom));
      // Rotation
      RotMatrix view_rotation;
      build_rotmatrix( view_rotation, _orient );
      rotation = glm::make_mat4x4( view_rotation );
      // Translation
      translation = glm::translate(  glm::mat4(1.0f),
                                     glm::vec3( _pos.x, _pos.y, 0.f));
      // Projection-View
      _view = zoom * translation * rotation;
      vp = _proj * _view;
      
      _viewer_grid.render( vp, {0.f, 0.f, 0.f} );

      // update token list
      _token_l.clear();
      for( auto& ptb: _boid_list) {
        _token_l.push_back( token_from_boid(ptb) );
      }
      
      _viewer_frame.render( vp /*projection*/ );
      _viewer_token.pre_render();
      _viewer_token.render_list( vp, _token_l );
      _viewer_token.post_render();
      // Some text, as GUI ?
      // Reset projection to 0,1 x 0,1
      glm::mat4 proj;
      proj = glm::ortho( 0.f, 1.f, // left;right
                         0.f, 1.f, // bottom,top
                         -1.f, 1.f // near far
                         );
      //glm::quat rot_txt = glm::rotate({0,0,0,1},
      // 								  (float) -M_PI/2.f,
      // 								  glm::vec3(1,0,0));
      // rotation using euler vectors
      glm::quat rot_txt = glm::quat(glm::vec3(0,0,M_PI));
      // TODO BUT, strangely, need to rotate over Oz!!!
      _gui_rect.render( proj,
                        {0.5f,0.95f,1.0f}, glm::quat(glm::vec3(0,0,0)),
                        {1.0f,0.1f,1.0f} );
      _gl_text.pre_render( proj, {0,0,1.0f}, rot_txt );
      _gl_text.set_scale( (1.f)/(float)_screen_width,
                          (1.f)/(float)_screen_height );
      _gl_text.set_color( {0.f, 0.f, 0.f, 1.f} );
      _gl_text.render( phy_ss.str(), 0.05f, 0.95f );
      _gl_text.render( fps_ss.str(), 0.05f, 0.01f );
      _gl_text.post_render();

      // Remove any programm so that glText can "work"
      //glUseProgram(0);

      glfwSwapBuffers(_window);
      glfwPollEvents();

      // clock
      auto end_proc = std::chrono::steady_clock::now();
      // wait period 
      std::chrono::duration<double> elapsed_seconds = end_proc - start_proc;
      std::this_thread::sleep_for(std::chrono::milliseconds(20)
                                  - elapsed_seconds );

      // Physics time
      _phys_time_cur = end_physics - start_proc;
      _phys_time_mean = 0.9 * _phys_time_mean + 0.1 * _phys_time_cur;
      // Frame time
      _frame_time_cur = end_proc - start_proc;
      _frame_time_mean = 0.9 * _frame_time_mean + 0.1 * _frame_time_cur;
      // save the current settings
      auto old_settings = fps_ss.flags(); //save previous format flags
      auto old_precision = fps_ss.precision();
    
      fps_ss << std::fixed;
      fps_ss.str("");
      fps_ss << "FPS c/M : ";
      fps_ss << std::setprecision(1);
      fps_ss << std::setw(5) <<_frame_time_cur.count() << "/";
      fps_ss << std::setw(5)<< _frame_time_mean.count();
      fps_ss << " PHYS c/m : ";
      fps_ss << std::setprecision(1);
      fps_ss << std::setw(5)<< (_phys_time_cur.count() / _frame_time_cur.count()) * 100.0 << "/";
      fps_ss << std::setw(5)<< (_phys_time_mean.count() / _frame_time_mean.count()) * 100.0 << " %";
      // restore output format flags and precision

      fps_ss.flags(old_settings);
      fps_ss.precision(old_precision);
    }
  }
  // *********************************************** GLBoidsSceen::get_clicked
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
  // **************************************************** GL3DSimu::final_state
  bool final_state() const { return _finished; };
  // ************************************************ GLBoidsScreen::attributs
  GLFWwindow* window() { return _window; };
private:
  /** Ptr sur la Fenetre */
  GLFWwindow* _window;
  int _screen_width=800, _screen_height=600;
  float _zoom;
  ScreenPos _start;
  ScreenPos _pos;
  Quaternion _orient;
  MouseAction _action;
  glm::mat4 _proj, _view;
  /** ready */
  bool _finished;
  /** Viewer */
  GL3DFrame _viewer_frame;
  GL3DGrid _viewer_grid;
  GLToken _viewer_token;
  /** GUI elements */
  GL3DTextShaders& _gl_text;
  GL3DRect _gui_rect;
  /** Simulations state */
  GLToken::TListToken _token_l;
  std::list<BoidBodyPtr> _boid_list;
  bool _physics_running;
  physics::Engine *_physics_eng;
  double _physics_time;
  /** Camera */
  /** FPS */
  std::chrono::duration<double, std::milli> _frame_time_cur;
  std::chrono::duration<double, std::milli> _frame_time_mean;
  std::chrono::duration<double, std::milli> _phys_time_cur;
  std::chrono::duration<double, std::milli> _phys_time_mean;
  // ****************************************************** GL3DSimu::callback
  // ************************************************* GLBoidsScreen::callback
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    // ESC
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GL_TRUE);
    }
    // else callback from Class
    else if( action == GLFW_PRESS or action == GLFW_REPEAT ) {
      //std::cout << "key_callback = " << key << std::endl;
      ((GLBoidsScreen *)glfwGetWindowUserPointer(window))->on_key_pressed( key, action );
    }
  }
  static void mouse_button_callback(GLFWwindow* window, int button,
				    int action, int mods)
  {
    // callback from Classe
    ((GLBoidsScreen *)glfwGetWindowUserPointer(window))->on_mouse_button( button, action, mods );
  }
  static void mouse_move_callback( GLFWwindow* window, 
				   double xpos, double ypos) 
  {
    // callback from Classe
    ((GLBoidsScreen *)glfwGetWindowUserPointer(window))->on_mouse_move( xpos, ypos);
  }
  static void scroll_callback(GLFWwindow* window,
			      double xoffset, double yoffset)
  {
    // callback from Classe
    ((GLBoidsScreen *)glfwGetWindowUserPointer(window))->on_scroll( yoffset );
  }
public:
  // ******************************************* GLBoidsScreen::on_key_pressed
  /** Called for GLFW_PRESSED and GLFW_REPEAT */
  void on_key_pressed( int key, int action ) 
  {
    
    //std::cout << "GLWindow::key_pressed key=" << key << std::endl;
    if( key == GLFW_KEY_R and action == GLFW_PRESS ) {
      // toggle _is_running
      _physics_running = !_physics_running;
    }
    else if( key == GLFW_KEY_E and action == GLFW_PRESS ) {
      // stop and reset
      _physics_running = false;
      physics_reset();
    }
  }
  // ****************************************** GLBoidsScreen::on_mouse_button
  void on_mouse_button( int button, int action, int mods ) 
  {
    double x, y;
    glfwGetCursorPos( _window, &x, &y);
    //std::cout <<"Mouse Button at (" << x <<  ", " << y << ")\n";

    if( action == GLFW_PRESS ) {
      if( button == GLFW_MOUSE_BUTTON_LEFT ) {
        // Infer the model that has been clicked.
        auto pt = glm::vec4{ (float)x, (float)y, 0.f, 1.f };
        if( get_mouse_horizontal( pt )){
          std::cout << " cliked on " << glm::to_string(pt) << std::endl;
        }
      }
      else if( button == GLFW_MOUSE_BUTTON_MIDDLE ) {
        // With SHIFT ??
        if( mods & GLFW_MOD_SHIFT ) {
          //_scene->mouse_action_start ("move-resize",x,y);
          //std::cout << "move-resize at " << x << ", " << y  << std::endl;
          _start = ScreenPos(x,y);
          _action = MouseAction::MOVE;
        }
        else if( mods & GLFW_MOD_CONTROL ) {
          //std::cout << "zoom at " << x << ", " << y  << std::endl;
        }
        else {
          //_scene->mouse_action_start ("rotate",x,y);
          _start = ScreenPos(x,y);
          _action = MouseAction::ROTATE;
          //std::cout << "rotate at " << x << ", " << y  << std::endl;
        }
      }
      else if( button == GLFW_MOUSE_BUTTON_RIGHT ) {
        //_scene->mouse_action_start( "zoom", x, y);
        _start = ScreenPos(x,y);
        _action = MouseAction::ZOOM;
        //std::cout << "btn right " << x << ", " << y  << std::endl;
      }
    }
    else if( action == GLFW_RELEASE ) {
      //_scene->mouse_action_end( x, y);
      _action = MouseAction::NOTHING;
      //std::cout << "end_action " << x << ", " << y  << std::endl;
      //std::cout << "           start= " << glm::to_string(_start) << std::endl;
    }
  }
  // ******************************************** GLBoidsScreen::on_mouse_move
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
  }
  // ************************************************ GLBoidsScreen::on_scroll
  /** yoffset vaut +/- 1 */
  void on_scroll( double yoffset ) 
  {
    //std::cout << "scroll_action yoffset=" << yoffset << std::endl;

    if( yoffset < 0 ) {
      _zoom *= ZOOM_COEF;
      if( _zoom > ZOOM_MAX ) _zoom = ZOOM_MAX;
    }
    else {
      _zoom /= ZOOM_COEF;
      if( _zoom < ZOOM_MIN ) _zoom = ZOOM_MIN;
    }
  }
};

#endif // GL_BOIDS_SCREEN_HPP
