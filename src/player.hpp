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

// ******************************************************************** GLOBAL
#define SPEED 0.3

// ***************************************************************************
// ******************************************************************** Player
// ***************************************************************************
class Player
{
public:
  // ******************************************************** Player::creation
  Player( const MColor& color = _col_blue ) :
    _color(color), _cursor_pos({-0.5, (double)color.index})
  {
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
  };
  // ******************************************************** Player::attribut
  const MColor& color() const { return _color; };
  const Vec2& cursor_pos() const { return _cursor_pos; };
  
private:
  /** Couleur */
  const MColor& _color;
  /** Curseur position */
  Vec2 _cursor_pos;
};

#endif // PLAYER_HPP
