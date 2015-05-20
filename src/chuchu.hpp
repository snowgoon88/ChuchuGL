/* -*- coding: utf-8 -*- */

#pragma once

/** 
 * Un Chuchu qui avance dans des Cell,
 * en sortant d'une Cell, obtient sa nouvelle Direction.
 */

#include <sstream>                        // std::stringstream

#include <global.hpp>
#include <cell.hpp>

// ***************************************************************************
// ******************************************************************** CHUCHU
// ***************************************************************************
class Chuchu {
public:
  // ****************************************************************** CREATION
  /** Creation avec une position et une vitesse */
  Chuchu( TVec2 position = {0,0}, Direction* dir = &_dir_right, TSpd speed = 0.1 ) :
    _pos(position), _dir(dir), _spd(speed), _cell(NULL)
  {};

  // *********************************************************************** STR
  /** Dump avec string */
  std::string str_dump()
  {
    std::stringstream dump;
    
    dump << "CHUCHU";
    dump << " at (" << _pos.x << "; " << _pos.y << ")";
    dump << " going to " << _dir->str << " with spd=" << _spd;

    return dump.str();
  };
  // ******************************************************************** UPDATE
  /** Mise à jour de la position */
  void update( double delta_t )
  {
    _pos.x += _dir->vec.x * _spd * delta_t; 
    _pos.y += _dir->vec.y * _spd * delta_t; 
  }

  // ****************************************************************** VARIABLE
  /** _pos */
  TVec2& pos() {return _pos;};
  void set_pos( const TPos& x, const TPos& y ) {_pos.x = x; _pos.y=y;};
  /** Quand on lui attribue une Cell, le Chuchu regarde s'il ENTER
   * ou EXIT cette CELL. 
   * En cas de EXIT, déterminer sa nouvelle _dir
   */
  void set_cell( Cell* cell )
  {
    _cell=cell;
    
    // Produit scalaire avec la cellule pour savoir si entre ou sort
    if( _cell ) {
      double pscal = (_pos.x - _cell->pos().x - 0.5) * _dir->vec.x +
	(_pos.y - _cell->pos().y - 0.5) * _dir->vec.y;
      if( pscal > 0.0 ) { // EXIT
	std::cout << "pscal=" << pscal << " : EXIT cell " << _cell->str_dump() << std::endl;
	// Et donc une nouvelle Direction
	_dir = _cell->dir_arrive_from( _dir );
      }
      else if( pscal < 0.0 ) { // ENTER
	std::cout << "pscal=" << pscal << " : ENTER cell " << _cell->str_dump() << std::endl;
      }
    }
  };
private:
  /** Position */
  TVec2 _pos;
  /** Direction actuelle */
  Direction* _dir;
  /** Vitesse */
  TSpd _spd;
  /** Cell sur laquelle se trouve le Chuchu */
  Cell* _cell;
};
