/* -*- coding: utf-8 -*- */

#ifndef CELL_CPP
#define CELL_CPP

/** 
 * Une Cell avec
 * - _type : string qui donne le type de case (CELL, ROCKET, SOURCE)
 * - _pos : position en bas à gauche de la case.
 * - _arrow = NULL ou &Direction : une flèche
 * - _wall : tableau de 4 boolean (présence/absence d'un mur)
 */

#include <sstream>                        // std::stringstream

#include "global.hpp"

class Chuchu;

// ***************************************************************************
// ********************************************************************** Cell
// ***************************************************************************
class Cell
{
public:
  // **************************************************************** creation
  /** Avec position */
  Cell( Vec2 position = {0,0} );
  Cell( const Cell& cell );
  // ********************************************************************* str
  /** Dump avec string */
  virtual std::string str_dump() const;
  // ******************************************************************* enter
  /** Return false si chuchu doit être détruit */
  virtual bool entered_by( const Chuchu& chu ) {return true;};
  // ***************************************************************** leaving
  /** Calcule la direction que l'on a en sortant de la Cell
   * quand on avait la direction dir.
   */
  Direction* dir_arrive_from( Direction* dir ) const;
  // *************************************************************** attributs
  const Vec2& pos() const {return _pos;};
  bool set_arrow( Direction* dir) {_arrow = dir; return true;}
  void add_wall( const Direction& dir ) {_wall[dir.index] = true;};
protected:
  /** Position */
  Vec2 _pos;
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
  // **************************************************************** creation
  Rocket( Vec2 position = {0,0} ) : Cell( position ), _count(0)
  { _type = "ROCKET";};
  Rocket( const Cell& cell ) : Cell(cell), _count(0)
  { _type = "ROCKET";};
  // ********************************************************************* str
  /** Dump avec string */
  virtual std::string str_dump()
  {
    std::stringstream dump;
    
    dump << Cell::str_dump() << " with " << _count << " chuchu";

    return dump.str();
  }
  // ******************************************************************* enter
  /** Return false si chuchu doit être détruit */
  virtual bool entered_by( const Chuchu& chu )
  { _count += 1; return false;};
  
  // *************************************************************** attributs
  bool set_arrow( Direction* dir) {return false;}
private:
  unsigned int _count;
};

// ***************************************************************************
// ******************************************************************** Source
// ***************************************************************************
class Source : public Cell
{
public:
  // **************************************************************** creation
  Source( Vec2 position = {0,0}, Direction* dir = &_dir_up) :
    Cell( position ), _dir(dir),
    _refract(1), _duration(0)
  { _type = "SOURCE"; };
  Source( const Cell& cell, Direction* dir = &_dir_up) : 
    Cell(cell), _dir(dir),
    _refract(1), _duration(0)
  { _type = "SOURCE"; };
  // ********************************************************************* str
  /** Dump avec string */
  virtual std::string str_dump()
  {
    std::stringstream dump;
    
    dump << Cell::str_dump() << " ref= " << _refract;

    return dump.str();
  };
  // ****************************************************************** update
  /** Return nullptr or a new Chuchu */
  Chuchu* update( double delta_t );
  // *************************************************************** attributs
private:
  /** Direction des Chuchu sortants */
  Direction* _dir;
  /** Periode refractaire */
  double _refract;
  /** _temps depuis le dernier Chuchu créé */
  double _duration;
};
#endif // CELL_CPP
