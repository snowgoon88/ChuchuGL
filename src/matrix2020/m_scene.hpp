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
  //TODO maybe copy, assign, etc
  // ********************************************************* Scene::creation
  Scene( Environment& env ) :
    _env(env)
  {
    _gobjects.clear();
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
  /** str_view 
   * display a view of the scene with Environment and GameObject
   */
  std::string str_view () const
  {
    std::stringstream view;

    // every possible Pos in Environment
    for( int row = (int)_env._nb_row-1; row >= 0; --row) {
      for( int col = 0; col < (int) _env._nb_col; ++col) {
        Pos pos( col, row );
        bool displayed = false;
        
        // Check GameObject
        for( auto& go: _gobjects) {
          if ( go->pos() == pos ) {
            view << '+';
            displayed = true;
          }
        }

        // Check environement
        if (not displayed) {
          if (_env.is_obstacle( pos )) {
            view << 'X';
          }
          else if (_env.is_cell( pos )) {
            view << '.';
          }
          else {
            view << ' ';
          }
        }
      }
      view << std::endl;
    }
    return view.str();
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
  void move( const Pos& dir )
  {
    Pos dest( _hacker->pos() + dir );
    if (dest.x < 0) dest.x = 0;
    if (dest.y < 0) dest.y = 0;
    
    Pos final_dest = dest; 
    if (_env.is_cell( final_dest )) {
      _hacker->_pos = final_dest;
    }
    else {
      std::cerr << "Game::move Invalid move to " << final_dest;
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
// *************************************************************** Scene - END

}; // namespace matrix2020


#endif // M_SCENE_HPP
