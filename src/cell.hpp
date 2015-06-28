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
#include <memory>                         // std::unique_ptr

#include <global.hpp>
#include <world.hpp>

// ***************************************************************************
// ********************************************************************** Cell
// ***************************************************************************
class Cell
{
public:
  // ********************************************************** Cell::creation
  /** Avec position */
  Cell( Vec2 position = {0,0} );
  Cell( const Cell& cell );
  // *************************************************************** Cell::str
  /** Dump avec string */
  virtual std::string str_dump() const;
  // ************************************************************* Cell::enter
  /** Return false si chuchu doit être détruit */
  virtual bool entered_by( const Chuchu& chu ) {return true;};
  // *********************************************************** Cell::leaving
  /** Calcule la direction que l'on a en sortant de la Cell
   * quand on avait la direction dir.
   */
  const Direction* dir_arrive_from( const Direction* dir ) const;
  // ********************************************************** Cell:attributs
  const Vec2& pos() const {return _pos;};
  /** Ajouter nullptr pour effacer arrow */
  bool set_arrow( const Direction* dir)
  {
    // tjs ok si nullptr
    if( dir == nullptr ) {
      _arrow = dir;
      return true;
    }
    // Sinon, déjà une Arrow -> false
    if( _arrow ) return false;
    // ok, on ajoute une nouvelle flèche
    _arrow = dir; return true;
  }
  void add_wall( const Direction& dir ) {_wall[dir.index] = true;};
  const Direction& arrow_dir() const {return *_arrow;};
protected:
  /** Position */
  Vec2 _pos;
  /** Arrow : NULL ou ref vers Direction */
  const Direction* _arrow;
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
  Rocket( const World &world, Vec2 position = {0,0} ) : 
    Cell( position ), _world(world), _count(0), _time_last(0)
  { _type = "ROCKET";};
  Rocket( const World &world, const Cell& cell ) : 
    Cell(cell), _world(world), _count(0), _time_last(0)
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
  {
    _time_last = _world.stime();
    _count += 1;
    return false;
  };
  
  // *************************************************************** attributs
  bool set_arrow( const Direction* dir) {return false;};
  double time_last() const { return _time_last; };
private:
  /** Lien vers le World */
  const World &_world;
  /** nb de chuchu */
  unsigned int _count;
  /** instant du dernier Chuchu créé */
  double _time_last;
};

// ***************************************************************************
// ******************************************************************** Source
// ***************************************************************************
class Source : public Cell
{
public:
  // ******************************************************** Source::creation
  Source( const World &world, 
	  Vec2 position = {0,0}, Direction* dir = &_dir_up) :
    Cell( position ), _dir(dir),
    _world(world), _refract(1), _time_last(0)
  { _type = "SOURCE"; };
  Source( const World &world,
	  const Cell& cell, Direction* dir = &_dir_up) : 
    Cell(cell), _dir(dir),
    _world(world), _refract(1), _time_last(0)
  { _type = "SOURCE"; };
  // ************************************************************* Source::str  
  /** Dump avec string */
  virtual std::string str_dump()
  {
    std::stringstream dump;
    
    dump << Cell::str_dump() << " ref= " << _refract;

    return dump.str();
  };
  // ********************************************************** Source::update
  /** Return nullptr or a new Chuchu */
  std::unique_ptr<Chuchu> update();
  // ******************************************************* Source::attributs
  bool set_arrow( const Direction* dir) {return false;};
private:
  /** Direction des Chuchu sortants */
  Direction* _dir;
  /** Ref au World, pour temps */
  const World &_world;
  /** Periode refractaire */
  double _refract;
  /** instant du dernier Chuchu créé */
  double _time_last;
};
#endif // CELL_CPP
