/* -*- coding: utf-8 -*- */

#ifndef GL_GAME_HPP
#define GL_GAME_HPP

/** 
 * GLGame implémente concept::GLScreen pour gérer une partie PvP.
 */

// Model
#include <world.hpp>

// Viewer
#include <gl_text.hpp>
#include <gl_engine.hpp>
#include <gl_world.hpp>
#include <gl_arrow.hpp>
#include <gl_controler.hpp>

#include <chrono>                      // std::chrono
#include <thread>                      // std::thread

// ******************************************************************** GLOBAL

// ***************************************************************************
// ******************************************************************** GLGame
// ***************************************************************************
class GLGame
{
public:
  // ******************************************************** GLGame::creation
  /** Création avec un GLEngine */
  GLGame( GLEngine& engine, World& world, GLControler& controler) :
    _window(engine.window()),
    _gl_texture(engine.gl_texture()),
    _gl_texture_fade(engine.gl_texture_fade()),
    _world(world), 
    _gl_world(engine,world), _gl_arrow(engine), _gl_text(),
    _controler(controler),
    _is_running(false), _anim_running(false),
    _frame_time_min(1000.0), _frame_time_max(0.0), _frame_time_avg(0.0)
  {
  };
  // ************************************************************ GLGame::init
  void init()
  {
    // Installe les callback pour les touches
    glfwSetWindowUserPointer( _window, this);
    glfwSetKeyCallback(_window, key_callback);

    std::cout << _world.str_dump() << std::endl;

  };
  // ********************************************************** GLGame::render
  void render()
  {
    // Transparence
    // Enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    _gl_text.set_glstate();

    // TODO cbk quand fenêtre redimensionnée
    // FPS
    auto frame_time = std::chrono::steady_clock::now();
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
      _controler.act( _window );
      
      // world update
      if( _is_running ) {
	_world.update( 0.020 );
      }

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
      _gl_arrow.pre_render();
      for( auto& assoc: _controler.assoc()) {
      	for( auto& arrow_cell: assoc.player->arrow()) {
      	  _gl_arrow.render_arrow( projection, arrow_cell->pos(),
      				  arrow_cell->arrow_dir(),
      				  assoc.player->color().index );
      	}
      	// Les cross des joueurs
      	_gl_arrow.render_cross( projection, assoc.player->cross_pos() );
      }
      _gl_arrow.post_render();
      
      // // Display cbk
      _gl_world.render( projection, anim_idx );
      

      // Les curseur des joueurs
      _gl_arrow.pre_render();
      for( auto& assoc: _controler.assoc()) {
	_gl_arrow.render_cursor( projection, assoc.player->cursor_pos(),
				 assoc.player->color().index, anim_idx );
      }
      _gl_arrow.post_render();
      
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
  };
  // ***************************************************** GLGame::final_state
  bool final_state() const { return true; };
  // ******************************************************* GLGame::attributs
private:
  /** Ptr sur la Fenetre */
  GLFWwindow* _window;
  int _screen_width, _screen_height;
  /** Shaders */
  const GLTexture&     _gl_texture;
  const GLTextureFade& _gl_texture_fade;
  /** Model */
  World& _world;
  /** Viewer */
  GLWorld _gl_world;
  GLArrow _gl_arrow;
  GLText _gl_text;
  /** Controler */
  GLControler& _controler;
  // ************************************************************** simulation
  bool _is_running;
  bool _anim_running;
  // ********************************************************************* fps
  std::chrono::duration<double, std::milli> _frame_time_min;
  std::chrono::duration<double, std::milli> _frame_time_max;
  std::chrono::duration<double, std::milli> _frame_time_avg;
  //***************************************************************** callback
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
      ((GLGame *)glfwGetWindowUserPointer(window))->on_key_pressed( key );
    }
  }
  // ********************************************************* public_callback
  void on_key_pressed( int key ) 
  {
    std::cout << "GLWindow::key_pressed key=" << key << std::endl;
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
      std::cout << _world.str_display() << std::endl;
    }
  };
  /**
   * Callback pour gérer les messages d'erreur de GLFW
   */
  static void error_callback(int error, const char* description)
  {
    std::cerr <<  description << std::endl;
  }
};

#endif // GL_GAME_HPP
