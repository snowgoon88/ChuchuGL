/* -*- coding: utf-8 -*- */

#ifndef M_SCENE_HPP
#define M_SCENE_HPP

/** 
 * Scene has an Environment and a GameObject
 */

#include <matrix2020/m_def.hpp>
#include <matrix2020/m_environment.hpp>
#include <matrix2020/m_gameobject.hpp>

#include <list>

namespace matrix2020 {

using GameObjectPtrC = std::list<GameObjectPtr>;
  
// ***************************************************************************
// ********************************************************************* Scene
// ***************************************************************************
class Scene
{
public:
  // ********************************************************* Scene::creation
  Scene( Environment& env ) :
    _env(env)
  {
    _gobjects.clear();
  }
  // ****************************************************** Scene::destruction
  virtual ~Scene()
  {
  }
  // ************************************************************ Scene::setup
  void init()
  {
    // Hacker at entrance
    _hacker = GameObjectPtr( new GameObject( "hacker" ) );
    _hacker->_pos = {4, 0};
    add( _hacker );
  }
  // ************************************************************** Scene::str
  std::string str_dump() const
  {
    std::stringstream dump;
    dump << "SCENE:" << std::endl;
    dump << _env.str_display();

    dump << "GameObjects:" << std::endl;
    for( auto& go: _gobjects ) {
      dump << go->str_dump() << std::endl;
    }

    return dump.str();
  }
  // ******************************************************* Scene::add_object
  /** add only if GameObject on a cell
   */
  bool add( GameObjectPtr obj)
  {
    if (_env.is_cell( obj->pos() )) {
      _gobjects.push_back( obj );
      return true;
    }
    else {
      std::cerr << "Scene::add GameObject in NOT cell";
      std::cerr << "( " << obj->pos().x << ", " << obj->pos().y << ")";
      std::cerr << std::endl;
    }
    return false;
  }
  // ************************************************************* Scene::move
  /** If possible, move hacker in 'dir'
   */
  void move( const Dir& dir )
  {
    Dir dest( {_hacker->pos().x + dir.x, _hacker->pos().y + dir.y } );
    if (dest.x < 0) dest.x = 0;
    if (dest.y < 0) dest.y = 0;
    
    Pos final_dest( {dest.x, dest.y} );
    if (_env.is_cell( final_dest )) {
      _hacker->_pos = final_dest;
    }
    else {
      std::cerr << "Game::move Invalid move to ";
      std::cerr << "( " << final_dest.x << ", " << final_dest.y << ")";
      std::cerr << std::endl;
    }
  }

  // ************************************************** Sceen::public_callback
  void on_key_up()
  {
    move( D_UP );
  }
  void on_key_down()
  {
    move( D_DOWN );
  }
  void on_key_left()
  {
    move( D_LEFT );
  }
  void on_key_right()
  {
    move( D_RIGHT );
  }
  void on_key_fire()
  {
  }
  // ******************************************************** Scene::attributs
  Environment& _env;
  GameObjectPtrC _gobjects;
  GameObjectPtr _hacker;
  
}; // Scene
}; // namespace matrix2020


#endif // M_SCENE_HPP
