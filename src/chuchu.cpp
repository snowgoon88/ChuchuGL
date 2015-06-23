// -*- coding: utf-8 -*-

#include "chuchu.hpp"

#include <iostream>
#include <sstream>                        // std::stringstream



// ****************************************************************** creation
Chuchu::Chuchu( Vec2 position, Direction* dir, Spd speed) :
  _pos(position), _dir(dir), _spd(speed), _cell(NULL), _is_exiting(false)
{}
// *********************************************************************** str
/** Dump avec string */
std::string Chuchu::str_dump() const
{
  std::stringstream dump;
  
  dump << "CHUCHU";
  dump << " at (" << _pos.x << "; " << _pos.y << ")";
  dump << " going to " << _dir->str << " with spd=" << _spd;
  dump << " _is_exiting=" << _is_exiting;
  
  return dump.str();
}
// ******************************************************************** update
/** Mise à jour de la position */
void Chuchu::update( double delta_t )
{
  _pos.x += _dir->vec.x * _spd * delta_t; 
  _pos.y += _dir->vec.y * _spd * delta_t; 
}
// ***************************************************************** attributs
/** 
 * Quand on lui attribue une Cell, le Chuchu regarde s'il ENTER
 * ou EXIT cette CELL. 
 * En cas de EXIT, déterminer sa nouvelle _dir
 *
 * @return : true si le chuchu vient de changer de Cell
 */
bool Chuchu::set_cell( Cell* cell )
{
  // Produit scalaire avec la cellule pour savoir si entre ou sort
  if( cell ) {
    double pscal = (_pos.x - (cell->pos().x+0.5)) * _dir->vec.x +
      (_pos.y - (cell->pos().y+0.5)) * _dir->vec.y;
    if( pscal > 0.0 ) { // EXIT
      std::cout << "pscal=" << pscal << " : EXIT cell " << cell->str_dump() << std::endl;
      // Et donc une nouvelle Direction s'il ne l'a pas déjà eu
      if( not _is_exiting ) 
	_dir = cell->dir_arrive_from( _dir );
      _is_exiting = true;
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
    _is_exiting = false;
    return true;
  }
  
  return false;
}
// ***************************************************************************
