// -*- coding: utf-8 -*-

#include "cell.hpp"
#include "chuchu.hpp"
#include <iostream>                       // std::cout
#include <sstream>                        // std::stringstream

// ************************************************************ Cell::creation

/** Avec position */
Cell::Cell( Vec2 position ) :
  _pos(position), _arrow(NULL), _wall{false, false, false, false},
  _type("CELL")
  {};
Cell::Cell( const Cell& cell ) :
  _pos(cell.pos()), _arrow(cell._arrow),
  _wall{cell._wall[0], cell._wall[1], cell._wall[2], cell._wall[3]},
  _type("CELL")
  {};
// ***************************************************************** Cell::str
/** Dump avec string */
std::string Cell::str_dump() const
{
  std::stringstream dump;
  
  dump << _type;
  dump << " at (" << _pos.x << "; " << _pos.y << ")";
  if( _arrow ) dump << " ARROW to " << _arrow->str;
  dump << " MUR en ";
  for( unsigned int i = 0; i < _dir_size; ++i) {
    if (_wall[i]) {
      dump << _dir_str(i);
    }
  }
  
  return dump.str();
};
// ************************************************************* Cell::leaving
/** Calcule la direction que l'on a en sortant de la Cell
 * quand on avait la direction dir.
 */
Direction* Cell::dir_arrive_from( Direction* dir ) const
{
  // Par défaut on va tout droit
  Direction* next_dir = dir;
  // Mais _arrow est prioritaire
  if(_arrow) {
    next_dir = _arrow;
  }
  // Ensuite, on regarde s'il y a des murs
  while( _wall[next_dir->index] ) {
    next_dir = _dir_rotate( next_dir );
  }
  return next_dir;
};
// ************************************************************ Source::update
/** Return nullptr or a new Chuchu */
std::unique_ptr<Chuchu> Source::update( double delta_t )
{
  _duration += delta_t;
  if( _duration > _refract ) {
    _duration = _duration - _refract;
    // Création d'un Chuchu
    return std::unique_ptr<Chuchu>(new Chuchu( {_pos.x+0.5, _pos.y+0.5}, _dir, 1.0));
  }
  else {
    return nullptr;
  }
};
