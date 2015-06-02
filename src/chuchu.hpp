/* -*- coding: utf-8 -*- */

#ifndef CHUCHU_CPP
#define CHUCHU_CPP

/** 
 * Un Chuchu qui avance dans des Cell.
 * en sortant d'une Cell, obtient sa nouvelle Direction.
 *
 * _pos : position sur le World => toujours positif. Un chuchu dont la 
 *        _pos.x et _pos.y sont entre 0 et 1 est dans la case (0.0).
 * 
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
  Chuchu( Vec2 position = {0,0}, Direction* dir = &_dir_right, Spd speed = 0.1 ) :
    _pos(position), _dir(dir), _spd(speed), _cell(NULL)
  {};

  // *********************************************************************** STR
  /** Dump avec string */
  std::string str_dump() const
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
  const Vec2& pos() const {return _pos;};
  void set_pos( const Pos& x, const Pos& y ) {_pos.x = x; _pos.y=y;};
  /** 
   * Quand on lui attribue une Cell, le Chuchu regarde s'il ENTER
   * ou EXIT cette CELL. 
   * En cas de EXIT, déterminer sa nouvelle _dir
   *
   * @return : true si le chuchu vient de changer de Cell
   */
  bool set_cell( Cell* cell )
  {
    // Produit scalaire avec la cellule pour savoir si entre ou sort
    if( cell ) {
      double pscal = (_pos.x - (cell->pos().x+0.5)) * _dir->vec.x +
	(_pos.y - (cell->pos().y+0.5)) * _dir->vec.y;
      if( pscal > 0.0 ) { // EXIT
	std::cout << "pscal=" << pscal << " : EXIT cell " << cell->str_dump() << std::endl;
	// Et donc une nouvelle Direction
	_dir = cell->dir_arrive_from( _dir );
	return true; // toujours vivant
      }
      else if( pscal < 0.0 ) { // ENTER
	std::cout << "pscal=" << pscal << " : ENTER cell " << cell->str_dump() << std::endl;
	//return cell->entered_cbk( this );
      }
    }

    // Mémorise la Cell actuelle et renvoir true si on vient 
    // de changer de Cell
    if( _cell != cell ) {
      _cell = cell;
      return true;
    }
    else {
      _cell = cell;
      return false;
    }
  };
  /** dir */
  const Direction& dir() const {return *_dir;};
private:
  /** Position */
  Vec2 _pos;
  /** Direction actuelle */
  Direction* _dir;
  /** Vitesse */
  Spd _spd;
  /** Cell sur laquelle se trouve le Chuchu */
  Cell* _cell;
};
#endif // CHUCHU_CPP
