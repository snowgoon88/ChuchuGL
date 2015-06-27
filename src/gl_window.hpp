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

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// Model
#include <world.hpp>
#include <player.hpp>
// Viewer
#include <gl_world.hpp>
#include "gl_arrow.hpp"
#include "gl_text.hpp"
// Contoler
#include <gl_controler.hpp>

#include <fstream>
#include <chrono>                      // std::chrono
#include <thread>                      // std::thread

// ******************************************************************** GLOBAL
#define ANIM_LENGTH 30
#define JOY_INDEX GLFW_JOYSTICK_1

typedef std::shared_ptr<World>        WorldPtr;
typedef std::shared_ptr<Player>       PlayerPtr;
typedef std::shared_ptr<GLControler>  GLControlerPtr;
// ***************************************************************************
// ****************************************************************** GLWindow
// ***************************************************************************
class GLWindow
{
public:
  // ****************************************************************** creation
  /** Création avec titre et taille de fenetre.*/
  GLWindow(const std::string& title = "GLFW Window", int width=800, int height=600, bool fullsize=false) :
    _screen_width(width), _screen_height(height),
    _player(nullptr), _controler(nullptr),
    _gl_world(nullptr), _world(nullptr), _is_running(false),
    _anim_running(false),
    _fg_joy_ready(false),
    _frame_time_min(1000.0), _frame_time_max(0.0), _frame_time_avg(0.0) 
  {
    std::cout << "Window creation" << std::endl;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    if( fullsize ) {
      _window = glfwCreateWindow(width, height, title.c_str(),
				 glfwGetPrimaryMonitor(), NULL);
    }
    else {
      _window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    }

    if (!_window) {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }
    // 'this' est associé à cette _window (pour les callback)
    glfwSetWindowUserPointer( _window, this);
    glfwMakeContextCurrent(_window);
    glfwSetKeyCallback(_window, key_callback);

    _gl_text.set_fontsize( 14 );
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
    // World
    _world = WorldPtr(new World( "data/world_6x5.json"));
    std::cout << _world->str_dump() << std::endl;
    // Joueurs
    _player = std::shared_ptr<Player>(new Player( _world, _col_blue ));
    //_world->init3x4();
    // Open file
    // std::ifstream myfile( "../data/world_6x5.json" );
    // _world->read_json( myfile );
    // myfile.close();
    _arrow_viewer = std::unique_ptr<GLArrow>(new GLArrow());
    _gl_world = std::unique_ptr<GLWorld>(new GLWorld( *_world ));
    
  };
  // void set_controler( GLControler::Type type )
  // {
  //   _controler = std::shared_ptr<GLControler>(new GLControler( _player, type ));
    
  //   switch( type ) {
  //   case GLControler::Type::KEYBOARD:
  //     // nothing to wait
  //     break;
  //   case GLControler::Type::XPAD:
  //     // Wait for joystick ready
  //     while( not _fg_joy_ready ) {
  // 	glfwJoystickPresent( GLFW_JOYSTICK_1 );
  // 	std::this_thread::sleep_for(std::chrono::milliseconds(100));
  // 	glfwPollEvents();
  //     }
  //     break;
  //   }
  // };
  // ************************************************* GLWindow::set_controler
  void set_controler()
  {
    _controler = std::make_shared<GLControler>(_window, _world);
    _controler->render();
    // Redonne les contrôles à la classe actuelle 
    // 'this' est associé à cette _window (pour les callback)
    glfwSetWindowUserPointer( _window, this);
    glfwSetKeyCallback(_window, key_callback);
  };
  // ******************************************************************** render
  void render ()
  {
    // Transparence
    // Enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    _gl_text.set_glstate();
    // Depth
    //glEnable( GL_DEPTH_TEST ); => empeche transparence

    // TODO cbk quand la fenètre est redimensionnée ??

    // joystick
    // int count; // nb_of buttons or axes
    // const unsigned char* state;
    // const float* axes;

    // FPS
    auto frame_time = std::chrono::steady_clock::now();
    //auto frame_time_fps1s = frame_time;
    //auto frame_time_fps5s = frame_time;
    std::stringstream fps_ss;
    fps_ss << "FPS m/avg/M : ";
    fps_ss << _frame_time_min.count() << "/";
    fps_ss << _frame_time_avg.count() << "/";
    fps_ss << _frame_time_max.count();

    // L'index d'animation va varier entre 0 et ANIM_LENGTH
    unsigned int anim_idx = 0;
    while (!glfwWindowShouldClose(_window)) {
      // clock
      auto start_proc = std::chrono::steady_clock::now();


      // TODO window pourrait être donné à la construction
      // du controler
      _controler->act( _window );
      // // joystick input
      // // bouton : Pose flèches
      // state = glfwGetJoystickButtons( JOY_INDEX, &count);
      // if( state[3] == GLFW_PRESS ) _player->put_arrow( _dir_up );
      // if( state[1] == GLFW_PRESS ) _player->put_arrow( _dir_right );
      // if( state[0] == GLFW_PRESS ) _player->put_arrow( _dir_down );
      // if( state[2] == GLFW_PRESS ) _player->put_arrow( _dir_left );
      // // direction
      // axes = glfwGetJoystickAxes( JOY_INDEX, &count );
      // if( axes[5] < -0.2 or axes[0] < -0.2) 
      // 	_player->move_cursor( _dir_left, 0.020 );
      // if( axes[5] > 0.2 or axes[0] > 0.2)
      // 	_player->move_cursor( _dir_right, 0.020 );
      // if( axes[6] > 0.2 or axes[1] > 0.2)
      // 	_player->move_cursor( _dir_down, 0.020 );
      // if( axes[6] < -0.2 or axes[1] < -0.2)
      // 	_player->move_cursor( _dir_up, 0.020 );
      
      // world update
      if( _is_running ) {
	_world->update( 0.020 );
      }
      // float ratio;
      // int width, height;
      
      glfwGetFramebufferSize(_window, &_screen_width, &_screen_height);
      //float ratio = _screen_width / (float) _screen_height;
      
      glViewport(0, 0, _screen_width, _screen_height);
      /* Clear the background as yellow #ffde94 */
      glClearColor(1.0, 1.0, 0.58, 1.0);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

      // do_ortho() - Needed by _gl_text
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();

      glOrtho(-1.0f, 10.0f, -1.0f, 10.0f, 1.f, -1.f);
      
      // Needed by _gl_text
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      

      // Projection (to 2D screen)
      glm::mat4 projection = glm::ortho( -1.0f, 10.0f,
					 -1.0f, 10.0f,
					 -1.0f, 1.0f );
      // Prépare GLText
      _gl_text.set_scale( (10.f + 1.f)/(float)_screen_width,
      (10.f + 1.f)/(float)_screen_height );
      glColor3f( 0.f, 0.f, 0.f );
      _gl_text.render( fps_ss.str(), 0.f, 9.f );

      // Les Arrow
      // TODO : couleur en fonction de joueurs !!
      // TODO : partager _arrow_viewer ??
      for( auto& assoc: _controler->assoc()) {
	for( auto& arrow_cell: assoc.player->arrow()) {
	  _arrow_viewer->render_arrow( projection, arrow_cell->pos(),
				       arrow_cell->arrow_dir(),
				       assoc.player->color().index );
	}
	// Les cross des joueurs
	_arrow_viewer->render_cross( projection, assoc.player->cross_pos() );
      }
      // Display cbk
      _gl_world->render( projection, anim_idx );

      for( auto& assoc: _controler->assoc()) {
	// Les curseur des joueurs
	_arrow_viewer->render_cursor( projection, assoc.player->cursor_pos(),
				      assoc.player->color().index, anim_idx );
      }

      anim_idx += 1;
      anim_idx = anim_idx % ANIM_LENGTH;

      // Remove any programm so that glText can "work"
      glUseProgram(0);

      glfwSwapBuffers(_window);
      glfwPollEvents();


      // clock
      auto end_proc = std::chrono::steady_clock::now();
      // wait period 
      std::chrono::duration<double> elapsed_seconds = end_proc - start_proc;
      std::this_thread::sleep_for(std::chrono::milliseconds(20)
				  - elapsed_seconds );
      //std::cout << "TIME = " <<  elapsed_seconds.count() << std::endl;
      //FPS update
      std::chrono::duration<double> frame_delta_time = end_proc - frame_time;
      if( _frame_time_min > frame_delta_time ) _frame_time_min = frame_delta_time;
      if( _frame_time_max < frame_delta_time ) _frame_time_max = frame_delta_time;
      _frame_time_avg = 0.9 * _frame_time_avg + 0.1 * frame_delta_time;
      frame_time = end_proc;
      
      fps_ss.str("");
      fps_ss << "FPS m/avg/M : ";
      fps_ss << _frame_time_min.count() << "/";
      fps_ss << _frame_time_avg.count() << "/";
      fps_ss << _frame_time_max.count();
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
    else if( key == GLFW_KEY_S) {
      // step anim
      _anim_running = true;
    }
    else if( key == GLFW_KEY_D) {
      // display les chuchu
      std::cout << _world->str_display() << std::endl;
    }
    else if( key == GLFW_KEY_J) {
      _fg_joy_ready = true;
    }
  };
  /**
   * Callback pour gérer les messages d'erreur de GLFW
   */
  static void error_callback(int error, const char* description)
  {
    std::cerr <<  description << std::endl;
  }
  // ***************************************************************** players
  PlayerPtr _player;
  GLControlerPtr _controler;
  std::unique_ptr<GLArrow>  _arrow_viewer;
  // ******************************************************************* world
  std::unique_ptr<GLWorld> _gl_world;
  WorldPtr   _world;
  // ************************************************************** simulation
  bool _is_running;
  bool _anim_running;
  bool _fg_joy_ready;
  // ********************************************************************* fps
  std::chrono::duration<double, std::milli> _frame_time_min;
  std::chrono::duration<double, std::milli> _frame_time_max;
  std::chrono::duration<double, std::milli> _frame_time_avg;
  GLText _gl_text;

};

#endif // GL_WINDOW_HPP
