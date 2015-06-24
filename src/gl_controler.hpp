/* -*- coding: utf-8 -*- */

#ifndef GL_CONTROLER_HPP
#define GL_CONTROLER_HPP

/** 
 * Fait le lien entre les input (clavier, pad)
 * et les actions d'un Player.
 */

// ******************************************************************** GLOBAL
#define JOY_INDEX GLFW_JOYSTICK_1

// ***************************************************************************
// *************************************************************** GLControler
// ***************************************************************************
class GLControler
{
public:
  // ***************************************************** GLControler::global
  typedef std::shared_ptr<Player> PlayerPtr;
  enum class Type {KEYBOARD,XPAD};

  // *************************************************** GLControler::creation
  GLControler( PlayerPtr player, Type type = Type::KEYBOARD ) :
    _player(player), _type(type)
  {
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
  // ************************************************** GLControler::attributs
private:
  /** Le Player qui agit */
  PlayerPtr _player;
  /** Le type de Contoler */
  Type _type;
};


#endif // GL_CONTROLER_HPP
