/* -*- coding: utf-8 -*- */

#ifndef PLAYER_HPP
#define PLAYER_HPP

/** 
 * Un Player a un Cursor à contrôler pour poser des Arrow.
 * _color : couleur du joueur.
 *
 * Réf sur World pour changer les cases en mettant des arrows.
 */

#include <global.hpp>
#include <world.hpp>

// ******************************************************************** GLOBAL
#define SPEED 5.0
#define NB_ARROW_MAX 3


typedef std::shared_ptr<World>   WorldPtr;
typedef std::list<CellPtr>       CArrowPtr;
// ***************************************************************************
// ******************************************************************** Player
// ***************************************************************************
class Player
{
public:
  // ******************************************************** Player::creation
  Player( WorldPtr world, const MColor& color = _col_blue) :
    _color(color), _world(world), 
    _cursor_pos({-0.5, (double)color.index}), _cross_pos({0,0})
  {
    _l_arrow.clear();
  };
  // ************************************************************* Player::str
  /** Dump avec string */
  std::string str_dump() const
  {
    std::stringstream dump;
    
    dump << "PLAYER " << _color.str;
    dump << " cursor at (" << _cursor_pos.x << "; " << _cursor_pos.y << ")";
  
    return dump.str();
  };
  // ********************************************************** Player::action
  void move_cursor( const Direction& dir,  double delta_t)
  {
    _cursor_pos.x += dir.vec.x * SPEED * delta_t; 
    _cursor_pos.y += dir.vec.y * SPEED * delta_t;

    CellPtr cell = _world->cell( cursor_pos() );
    if( cell ) {
      _cross_pos = cell->pos();
    }
  };
  /** Pose Arrow au cursor */
  void put_arrow( Direction& dir )
  {
    put_arrow( dir, cursor_pos() );
  }
  /** Pose Arrow à un endroit */
  void put_arrow( Direction& dir, const Vec2& pos )
  {
    CellPtr place = _world->cell( pos );
    if( place->set_arrow( &dir )) {
      _l_arrow.push_back( place );
      if( _l_arrow.size() > NB_ARROW_MAX ) {
	// Efface la première Arrow de la liste
	_l_arrow.front()->set_arrow( nullptr );
	_l_arrow.pop_front();
      }
    }
  };
  // ******************************************************** Player::attribut
  const MColor& color() const { return _color; };
  const Vec2& cursor_pos() const { return _cursor_pos; };
  const Vec2& cross_pos() const { return _cross_pos; };
  const CArrowPtr& arrow() const { return _l_arrow; };
private:
  /** Couleur */
  const MColor& _color;
  /** World */
  WorldPtr _world;
  /** Curseur position */
  Vec2 _cursor_pos;
  /** Cross position */
  Vec2 _cross_pos;
  /** Arrow restantes */
  CArrowPtr _l_arrow;
};

#endif // PLAYER_HPP
