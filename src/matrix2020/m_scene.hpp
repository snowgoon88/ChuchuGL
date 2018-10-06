/* -*- coding: utf-8 -*- */

#ifndef M_SCENE_HPP
#define M_SCENE_HPP

/** 
 * Scene has an Environment and a GameObject
 */

#include <matrix2020/m_def.hpp>
#include <matrix2020/m_environment.hpp>
#include <matrix2020/m_gameobject.hpp>
#include <matrix2020/m_agent.hpp>

#include <list>

namespace matrix2020 {

using GameObjectPtrC = std::list<GameObjectPtr>;
  
// ***************************************************************************
// ********************************************************************* Scene
// ***************************************************************************
class Scene
{
  using Cursor = struct s_Cursor {
    Pos pos;
    bool visible;
  };
public:
  //TODO maybe copy, assign, etc
  // ********************************************************* Scene::creation
  Scene( Environment& env ) :
    _env(env), _gobjects(), _hacker(nullptr),
    _cursor( {{0, 0}, false} )
  {
    _gobjects.clear();
    init();
  }
  // ******************************************************* Scene::destructor
  virtual ~Scene()
  {
    _gobjects.clear();
  }
  // ************************************************************ Scene::setup
  void init()
  {
    // Hacker at entrance
    _hacker = GameObjectPtr( new GameObject( "hacker" ) );
    _hacker->pos( {4, 0} );
    //add( _hacker );

    // Agent at (1,5)
    auto agent = GameObjectPtr( new Agent( _env, "A1" ) );
    agent->pos( {1, 5} );
    add( agent );

    // Agent at (0,4)
    agent = GameObjectPtr( new Agent( _env, "A2" ) );
    agent->pos( {4, 0} );
    add( agent );
  }
  // ************************************************************** Scene::str
  std::string str_dump() const
  {
    std::stringstream dump;
    dump << "SCENE:" << std::endl;
    dump << _env.str_display();
    dump << "Hacker: " << _hacker->pos() << std::endl;
    
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

        // Check Hacker
        if (_hacker->pos() == pos ) {
          view << "*";
          displayed = true;
        }

        if (not displayed) {
          // Check GameObject
         for( auto& go: _gobjects) {
           if ( go->pos() == pos ) {
             view << '+';
             displayed = true;
           }
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
  template<class T>
  void valid_move( T& obj, Pos dest)
  {
    // TODO: should also test if too large x and y
    if (dest.x < 0) dest.x = 0;
    if (dest.y < 0) dest.y = 0;
    
    if (_env.is_cell( dest )) {
      obj.pos( dest );
    }
    else {
      std::cerr << "Game::move Invalid move of " << obj._name << "to " << dest;
      std::cerr << std::endl;
    }
  }
  void move_hacker( const Pos& dir )
  {
    _hacker->action( {_hacker->pos() + dir, dir} );
  }
  /** All Object are moved if valid action */
  void move_all()
  {
    // Hacker
    valid_move( *_hacker, _hacker->action().pos );
    
    for( auto& obj: _gobjects) {
      // GameObject decides where it would like to go
      obj->take_decision( true );

      // Move if valid
      Pos dest = obj->action().pos;
      valid_move<GameObject>( *obj, dest );
      obj->orient( obj->action().dir );
    }

  }
  // *********************************************************** Scene::cursor
  void set_cursor_visible( bool visible )
  {
    _cursor.visible = visible;
  }
  void move_cursor( const Pos& dir )
  {
    if (not _cursor.visible) return;
    
    _cursor.pos += dir;
    // keep cursor in scene (torus world)
    if ( _cursor.pos.x < 0 ) _cursor.pos.x = (int) _env._nb_col;
    if ( _cursor.pos.x > (int) _env._nb_col ) _cursor.pos.x = 0;
    if ( _cursor.pos.y < 0 ) _cursor.pos.y = (int) _env._nb_row;
    if ( _cursor.pos.y > (int) _env._nb_row ) _cursor.pos.y = 0;
  }
  // ************************************************** Sceen::public_callback
  void on_key_up()
  {
    move_hacker( D_UP );
    move_all();
  }
  void on_key_down()
  {
    move_hacker( D_DOWN );
    move_all();
  }
  void on_key_left()
  {
    move_hacker( D_LEFT );
    move_all();
  }
  void on_key_right()
  {
    move_hacker( D_RIGHT );
    move_all();
  }
  void on_key_fire()
  {
    move_all();
  }
  void on_cursor_up()     { move_cursor( D_UP ); }
  void on_cursor_down()   { move_cursor( D_DOWN ); }
  void on_cursor_right()  { move_cursor( D_RIGHT ); }
  void on_cursor_left()   { move_cursor( D_LEFT ); }
  void on_cursor_switch() { set_cursor_visible( not _cursor.visible ); }
  // ******************************************************** Scene::attributs
  Environment& _env;
  GameObjectPtrC _gobjects;
  GameObjectPtr _hacker;
  Cursor _cursor;
}; // Scene
// *************************************************************** Scene - END

}; // namespace matrix2020


#endif // M_SCENE_HPP
