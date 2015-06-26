/* -*- coding: utf-8 -*- */

#ifndef GL_CONTROLER_HPP
#define GL_CONTROLER_HPP

/** 
 * Fait le lien entre les différents inputs (clavier, pad)
 * et les actions des Players.
 */

// OpenGL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <GLFW/glfw3.h>

#include <sstream>            // std::stringstream
#include <string>             // std::string

// Model
//#include <world.hpp>
#include <player.hpp>
// Viewer
//#include <gl_world.hpp>
//#include "gl_arrow.hpp"
#include "gl_text.hpp"

// ******************************************************************** GLOBAL
#define JOY_INDEX GLFW_JOYSTICK_1
#define NB_MAX_PLAYER 2
// ***************************************************************************
// *************************************************************** GLControler
// ***************************************************************************
class GLControler
{
public:
  // ***************************************************** GLControler::global
  typedef std::shared_ptr<Player> PlayerPtr;
  typedef std::shared_ptr<World>   WorldPtr;
  enum class Type {NOTHING,KEYBOARD,XPAD};
  
  typedef struct {
    PlayerPtr player;
    Type      choice;
    int       idx_joy;
  } Association;
  typedef struct {
    GLboolean            present;
    std::string          name;
    int                  button_count;
    const unsigned char* buttons;
  } Joystick;

  // *************************************************** GLControler::creation
  GLControler( GLFWwindow* window,  WorldPtr world) :
    _world(world), _ready(false), _window(window)
  {
    // Pas de joueur
    _l_assoc.clear();
    _player_focus = 0;

    // Les Joystick
    for( unsigned int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; ++i) {
      _l_joy[i].present = GL_FALSE;
    }
    _nb_joy = 0;

    // 'this' est associé à cette _window (pour les callback)
    glfwSetWindowUserPointer( _window, this);
    glfwSetKeyCallback(_window, key_callback);

  };
  // ***************************************************** GLControler::detect
  /** Detecte les Joystick présents */
  void detect()
  {
    _nb_joy = 0;
    for( unsigned int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; ++i) {
      // Présent ?
      if( glfwJoystickPresent( i )) {
	_nb_joy++;
	_l_joy[i].present = GL_TRUE;
	_l_joy[i].name = std::string(glfwGetJoystickName(i));
	_l_joy[i].buttons = glfwGetJoystickButtons(i, &(_l_joy[i].button_count));
      }
      else if( _l_joy[i].present ) { // Etait détecté avant
	_l_joy[i].present = GL_FALSE;
	_l_joy[i].name = std::string("-");
      }	
    }
  };
  // ******************************************************** GLControler::act
  /** En fonction de Input, déclenche les actions */
  void act( GLFWwindow *window )
  {
    // glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

    switch(_type) {
    case Type::KEYBOARD:
      // Pose de flèches
      if( glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ) 
	 _player->put_arrow( _dir_up );
      if( glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ) 
	 _player->put_arrow( _dir_right );
      if( glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ) 
	 _player->put_arrow( _dir_down );
      if( glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ) 
	 _player->put_arrow( _dir_left );
      // Move Cursor
      if( glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS ) 
	_player->move_cursor( _dir_up, 0.020 );
      if( glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS ) 
	_player->move_cursor( _dir_right, 0.020 );
      if( glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS ) 
	_player->move_cursor( _dir_down, 0.020 );
      if( glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS ) 
	_player->move_cursor( _dir_left, 0.020 );
      break;
    case Type::XPAD:
      int count; // nb_of buttons or axes
      const unsigned char* state;
      const float* axes;
      // bouton : Pose flèches
      state = glfwGetJoystickButtons( JOY_INDEX, &count);
      if( state[3] == GLFW_PRESS ) _player->put_arrow( _dir_up );
      if( state[1] == GLFW_PRESS ) _player->put_arrow( _dir_right );
      if( state[0] == GLFW_PRESS ) _player->put_arrow( _dir_down );
      if( state[2] == GLFW_PRESS ) _player->put_arrow( _dir_left );
      // direction
      axes = glfwGetJoystickAxes( JOY_INDEX, &count );
      if( axes[5] < -0.2 or axes[0] < -0.2) 
	_player->move_cursor( _dir_left, 0.020 );
      if( axes[5] > 0.2 or axes[0] > 0.2)
	_player->move_cursor( _dir_right, 0.020 );
      if( axes[6] > 0.2 or axes[1] > 0.2)
	_player->move_cursor( _dir_down, 0.020 );
      if( axes[6] < -0.2 or axes[1] < -0.2)
	_player->move_cursor( _dir_up, 0.020 );
      break;
    };
  };
  // ******************************************************* GLControler::text
  void render_text( float topx, float topy )
  {
    // Joueurs
    _gl_text.set_fontsize( 20 );
    _gl_text.render( "CHOIX DES JOUEURS et des CONTROLEURS", topx, topy );
    topy -= _gl_text.line_height();
    _gl_text.set_fontsize( 16 );
    _gl_text.render( "     a/z : ajoute/enlève des joueurs", topx, topy );
    topy -= _gl_text.line_height();
    _gl_text.render( "     UP/DOWN : choix du joueur  LEFT/RIGHT : choix du controler", topx, topy );
    topy -= _gl_text.line_height();
    _gl_text.render( "     ENTER : sélection terminée", topx, topy );
    topy -= _gl_text.line_height();
    topy -= _gl_text.line_height();

    _gl_text.set_fontsize( 20 );
    _gl_text.render( "JOUEURS", topx, topy );
    topy -= _gl_text.line_height();
    _gl_text.set_fontsize( 16 );

    unsigned int idx_player = 0;
    for( auto& assoc: _l_assoc) {
      std::stringstream buf;
      if( idx_player == _player_focus ) 
	buf << " ***";
      else
	buf << "    ";
      buf << "Player " <<  assoc.player->color().str;
      buf << " => ";
      switch( assoc.choice ) {
      case Type::NOTHING:
	buf << "NO controler";
	break;
      case Type::KEYBOARD:
	buf << "Clavier";
	break;
      case Type::XPAD:
	if( _l_joy[assoc.idx_joy].present ) 
	  buf << "PAD [" << _l_joy[assoc.idx_joy].name << "]";
	else {
	  assoc.choice = Type::NOTHING;
	  assoc.idx_joy = 0;
	}   
      }
      if( idx_player == _player_focus ) 
	buf << " ***";
      else
	buf << "    ";
      _gl_text.render( buf.str(), topx, topy );
      topy -= _gl_text.line_height();
      idx_player++;
    } 
    topy -= _gl_text.line_height(); // endl

    // Joystick
    _gl_text.set_fontsize( 20 );
    _gl_text.render( "JOYSTICK DETECTES", topx, topy );
    topy -= _gl_text.line_height();
    _gl_text.set_fontsize( 16 );
    for( unsigned int i = 0; i < GLFW_JOYSTICK_LAST; ++i) {
      if( _l_joy[i].present ) {
	std::stringstream buf;
	buf << "     " << i << " : " << _l_joy[i].name << " => ";
	for( unsigned int j = 0;
	     j < (unsigned int) _l_joy[i].button_count; ++j) {
	  if( _l_joy[i].buttons[j] ) buf << "X";
	  else buf << ".";
	}
	_gl_text.render( buf.str(), topx, topy );
	topy -= _gl_text.line_height();
      }
    }
  }
  // ***************************************************** GLControler::render
  void render ()
  {
    // Transparence
    // Enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    _gl_text.set_glstate();

    while( not _ready && !glfwWindowShouldClose(_window)) {
      // Détecte Joystick et prépare message
      detect();

      glfwGetFramebufferSize(_window, &_screen_width, &_screen_height);
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

      // Prépare GLText
      _gl_text.set_scale( (10.f + 1.f)/(float)_screen_width,
			  (10.f + 1.f)/(float)_screen_height );
      glColor3f( 0.f, 0.f, 0.f );
      render_text( 0.f, 9.f );

      glfwSwapBuffers(_window);
      glfwPollEvents();
    }
  };
  // ************************************************** GLControler::attributs
private:
  /** Ref vers World */
  WorldPtr _world;
  /** Liste des Association Player-Controler */
  std::vector<Association> _l_assoc;
  unsigned int _player_focus;
  /** Liste des Joystick présent */
  Joystick _l_joy[GLFW_JOYSTICK_LAST - GLFW_JOYSTICK_1 + 1];
  unsigned int _nb_joy;
  /** Le Player qui agit */
  PlayerPtr _player;
  /** Le type de Contoler */
  Type _type;
  /** Choix fait ? */
  bool _ready;
  // *************************************************** GLControler::callback
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    // ESC => Quit
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
    // sinon callback de la classe
    else if( action == GLFW_PRESS ) {
      // std::cout << "key_callback = " << key << std::endl;
      ((GLControler*)glfwGetWindowUserPointer(window))->on_key_pressed( key );
    }
  };
  // ******************************************** GLControler::public_callback
  void on_key_pressed( int key ) 
  {
    // Ajout de Joueur
    if( key == GLFW_KEY_Q) {
      if( _l_assoc.size() < NB_MAX_PLAYER ) {
	// Association assoc =  
	_l_assoc.push_back( {PlayerPtr(new Player(_world, _l_col[_l_assoc.size()])),
	      Type::NOTHING, 0}  );
      }
    }
    // Enlève le dernier joueur
    if( key == GLFW_KEY_W) {
      _l_assoc.pop_back();
    }
    // Déplace parmis les joueurs
    // //std::cout << "on_key_presssed = " << key << std::endl;
    if( key == GLFW_KEY_UP) {
      _player_focus = (_player_focus + 1) % _l_assoc.size();
    }
    else if( key == GLFW_KEY_DOWN) {
      _player_focus = (_player_focus + 1) % _l_assoc.size();
    }
    // Switch entre les Contrôleur
    else if( key == GLFW_KEY_LEFT) {
      Association& assoc = _l_assoc[_player_focus];
      switch( assoc.choice ) {
      case Type::KEYBOARD:
	assoc.choice = Type::NOTHING;
	break;
      case Type::NOTHING:
	if( _nb_joy > 0 ) {
	  assoc.choice = Type::XPAD;
	  assoc.idx_joy = _nb_joy-1;
	}
	else {
	  assoc.choice = Type::KEYBOARD;
	  assoc.idx_joy = 0;
	}
	break;
      case Type::XPAD:
	assoc.idx_joy -= 1;
	if( assoc.idx_joy < 0 ) {
	  assoc.choice = Type::KEYBOARD;
	  assoc.idx_joy = 0;
	}
      }
    }
    else if( key == GLFW_KEY_RIGHT) {
      Association& assoc = _l_assoc[_player_focus];
      switch( assoc.choice ) {
      case Type::NOTHING:
	assoc.choice = Type::KEYBOARD;
	break;
      case Type::KEYBOARD:
	if( _nb_joy > 0 ) {
	  assoc.choice = Type::XPAD;
	}
	else {
	  assoc.choice = Type::NOTHING;
	}
	assoc.idx_joy = 0;
	break;
      case Type::XPAD:
	assoc.idx_joy += 1;
	if( assoc.idx_joy >= (int) _nb_joy ) {
	  assoc.choice = Type::NOTHING;
	  assoc.idx_joy = 0;
	}
      }
    }
    else if( key == GLFW_KEY_ENTER) {
      _ready = true;
    }
  };
  // ***************************************************** GLControler::viewer
  /** Ptr sur la Fenetre */
  GLFWwindow* _window;
  int _screen_width=800, _screen_height=600;
  GLText _gl_text;
};


#endif // GL_CONTROLER_HPP
