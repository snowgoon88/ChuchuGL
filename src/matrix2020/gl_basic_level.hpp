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
#include <matrix2020/fov_hamming.hpp>
#include <list>
// Viewer
#include <matrix2020/gl_environment.hpp>
#include <matrix2020/gl_hacker.hpp>
#include <matrix2020/gl_fovhamming.hpp>
#include <matrix2020/gl_agent.hpp>
#include <matrix2020/gl_cursor.hpp>
#include <matrix2020/gl_overlay.hpp>
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
    _scene(nullptr), _hacker_pos( {0,0} ), _fov(nullptr),
    _gl_env(nullptr), _gl_hacker(nullptr), _gl_fov(nullptr), _gl_agent(nullptr),
    _gl_cursor(nullptr), _gl_overlay(nullptr)
  {
  }
  // ************************************************ GLBasicLevel::destructor
  virtual ~GLBasicLevel()
  {
    if (_scene) delete _scene;
    if (_fov) delete _fov;

    if (_gl_env) delete _gl_env;
    if (_gl_hacker) delete _gl_hacker;
    if (_gl_fov) delete _gl_fov;
    if (_gl_agent) delete _gl_agent;
    if (_gl_cursor) delete _gl_cursor;
  }
  // ****************************************************** GLBasicLevel::init
  void init()
  {
    // Installe les callback pour les touches
    glfwSetWindowUserPointer( _window, this);
    glfwSetKeyCallback(_window, key_callback);
    glfwSetMouseButtonCallback( _window, mouse_button_callback );
    
    // Init Scene
    _env.load_from_txt( "data/matrix00.txt" );
    _scene = new Scene( _env );
    _scene->_hacker->pos( {5,5} );
    _hacker_pos = _scene->_hacker->pos();
    _fov = new FOVHamming( _env, _hacker_pos, 4 /* size of fov */ ); 

    std::cout << _scene->str_dump() << std::endl;
    
    // And graphic object
    _gl_env = new GLEnvironment( _env );
    _gl_hacker = new GLHacker( _scene->_hacker );
    _gl_fov = new GLFovHamming( *_fov );
    _gl_agent = new GLAgent( _scene->_gobjects );
    _gl_cursor = new GLCursor( _scene->_cursor.pos );
    _gl_overlay = new GLOverlay();
    // add a first message, framed in red
    auto ovr = _gl_overlay->add_overlay( 0.0, 0.0, {1.0, 0.0, 0.0} );
    ovr->add_text( "__MSG OVERLAY", {1.0, 0.0, 0.0} );
    ovr->add_text( " Hello", {1.0, 1.0, 1.0} );
    // second message, framed in green
    ovr = _gl_overlay->add_overlay( 0.0, 0.0, {0.0, 1.0, 0.0} );
    ovr->add_text( "__MSG OVERLAY", {0.0, 1.0, 0.0} );
    ovr->add_text( " How are you ?", {1.0, 1.0, 1.0} );
  }
  // **************************************************** GLBasicLevel::render
  void render()
  {
    // Transparence
    // Enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Camera mode : static
    _proj = glm::mat4( 1.0f );
    _proj = glm::ortho( -3.f, 15.f, // left;right
                        -3.f, 11.f, // bottom,top
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
      // TODO: BEURK
      _hacker_pos =_scene->_hacker->pos();
      _fov->refresh();
      _gl_fov->update_data();
      
      // Render
      _gl_env->render( _projview );
      _gl_agent->render( _projview );
      _gl_fov->render( _projview );
      _gl_hacker->render( _projview );

      // TODO: this changes the projection (side effect on projection )
      _gl_overlay->render( _projview, sx, sy );

      if (_scene->_cursor.visible) _gl_cursor->render( _projview );
      
      glfwSwapBuffers(_window);
      glfwPollEvents();
    }        
  }
  // *********************************************** GLBasicLevel::final_state
  bool final_state() const { return true; }
  // ************************************************* GLBasicLevel::attributs
  GLFWwindow* _window;
  int _screen_width, _screen_height;
  glm::mat4 _proj, _view, _projview;
  
  Environment  _env;
  Scene *      _scene;
  Pos          _hacker_pos;
  FOVHamming*  _fov;

  GLEnvironment* _gl_env;
  GLHacker*      _gl_hacker;
  GLFovHamming*  _gl_fov;
  GLAgent*       _gl_agent;
  GLCursor*      _gl_cursor;
  GLOverlay*     _gl_overlay;
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
    else if (key == GLFW_KEY_W)     _scene->on_cursor_up();
    else if (key == GLFW_KEY_S)     _scene->on_cursor_down();
    else if (key == GLFW_KEY_A)     _scene->on_cursor_left();
    else if (key == GLFW_KEY_D)     _scene->on_cursor_right();
    else if (key == GLFW_KEY_SPACE) _scene->on_cursor_switch();
    else if (key == GLFW_KEY_E)     print_info_cursor();

    std::cout << _scene->str_dump() << std::endl;
  }
  // ***************************************************** GLBasicLevel::mouse
  static void mouse_button_callback(GLFWwindow* window, int button,
				    int action, int mods)
  {
    // callback from Classe
    ((GLBasicLevel *)glfwGetWindowUserPointer(window))->on_mouse_button( button, action, mods );
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
          std::cout << "  cliked on " << glm::to_string(pt) << std::endl;
          std::cout << "  pos=" << world_to_env(pt) << std::endl;

          _scene->set_cursor( world_to_env(pt) );
          print_info_cursor();
        }
      }
    }
  }
  // *********************************************** GLBasicLevel::get_clicked
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
    std::cout << "  computed at " << glm::to_string(pt_mouse) << std::endl;
    
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
  // ***************************************** GLBasicLevel::print_info_cursor
  void print_info_cursor()
  {
    if (_scene->_cursor.visible) {
      std::cout << "__INFO " << std::endl;
      auto objects = _scene->get_obj_cursor();
      for( auto& obj : objects ) {
        std::cout << "  " << obj->str_dump() << std::endl;
      }
    }
  }
  // ********************************************** GLBasicLevel::world_to_env
  /** transform word coordinate to environment/scene coordinate */
  Pos world_to_env( const glm::vec4& world_pt )
  {
    // Interested in x and y only
    Pos env_pt = { static_cast<int>( world_pt.x+0.5f ),
                   static_cast<int>( world_pt.y+0.5f )};
    return env_pt;
  }
}; // GLBasicLevel

#endif // GL_BASIC_SCENE_HPP
