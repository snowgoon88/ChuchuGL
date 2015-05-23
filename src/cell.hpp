/* -*- coding: utf-8 -*- */

#ifndef CELL_CPP
#define CELL_CPP

/** 
 * Une Cell avec
 * - flèche : _arrow = NULL ou &Direction
 * - murs : TODO ---> tableau de 4 boolean ??
 */

#include <sstream>                        // std::stringstream

#include <global.hpp>
class Chuchu;

// ***************************************************************************
// ********************************************************************** Cell
// ***************************************************************************
class Cell
{
public:
  // ****************************************************************** creation
  /** Avec position */
  Cell( TVec2 position = {0,0} ) :
    _pos(position), _arrow(NULL), _wall{false, false, false, false},
    _type("CELL")
  {};

  // *********************************************************************** str
  /** Dump avec string */
  virtual std::string str_dump()
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
  // ********************************************************************* enter
  /** Return false si chuchu doit être détruit */
  virtual bool entered_by( const Chuchu& chu ) {return true;};
  // ******************************************************************* leaving
  /** Calcule la direction que l'on a en sortant de la Cell
   * quand on avait la direction dir.
   */
  Direction* dir_arrive_from( Direction* dir )
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
  // ***************************************************************** attributs
  TVec2& pos() {return _pos;};
  bool set_arrow( Direction* dir) {_arrow = dir; return true;}
  void add_wall( const Direction& dir ) {_wall[dir.index] = true;};
protected:
  /** Position */
  TVec2 _pos;
  /** Arrow : NULL ou ref vers Direction */
  Direction* _arrow;
  /** Murs */
  bool _wall[_dir_size];
  /** Type de case */
  std::string _type;
};

// ***************************************************************************
// ******************************************************************** Rocket
// ***************************************************************************
class Rocket : public Cell
{
public:
  // ****************************************************************** creation
  Rocket( TVec2 position = {0,0} ) : Cell( position ), _count(0)
  { _type = "ROCKET";};
  /** Dump avec string */
  virtual std::string str_dump()
  {
    std::stringstream dump;
    
    dump << Cell::str_dump() << " with " << _count << " chuchu";

    return dump.str();
  }
  // ********************************************************************* enter
  /** Return false si chuchu doit être détruit */
  virtual bool entered_by( const Chuchu& chu )
  { _count += 1; return false;};
  
  // ***************************************************************** attributs
  bool set_arrow( Direction* dir) {return false;}
private:
  unsigned int _count;
};
#endif // CELL_CPP
