/* -*- coding: utf-8 -*- */

#ifndef WORLD_CPP
#define WORLD_CPP

/** 
 * Le World est composé de Cell et de Chuchu.
 * Il y a _nb_row x _nb_col Cell
 *
 * Actuellement _l_cell[idx+idy*_nb_col] est en (idx,idy)
 * - 5 case de (0,0) à (5,0)
 * - flèche_gauche dans (0,0)
 * - flèche_droite dans (2,0)
 */

#include <sstream>                        // std::stringstream
#include <fstream>                        // std::ifstream
#include <vector>                         // std::vector
#include <list>                           // std::list
#include <memory>                         // std::unique_ptr

#include <global.hpp>

// ******************************************************************** GLOBAL
class Cell;  typedef std::shared_ptr<Cell>       CellPtr;
class Rocket;typedef std::shared_ptr<Rocket>     RocketPtr;
class Source;typedef std::shared_ptr<Source>     SourcePtr;
#include <chuchu.hpp>
typedef std::shared_ptr<Chuchu>     ChuchuPtr;

typedef std::vector<CellPtr>     CCellPtr;
typedef std::list<RocketPtr>     CRocketPtr;
typedef std::list<SourcePtr>     CSourcePtr;

// ***************************************************************************
// ********************************************************************* WORLD
// ***************************************************************************
class World {
public:
  // ********************************************************** World::typedef
  typedef struct {
    Pos x, y;
    unsigned int idd; // index direction
  } Wall;
public:
  // ********************************************************* World::creation
  World() : _stime(0) {};
  World( std::string filename ) : _stime(0)
  { 
    std::ifstream myfile( filename );
    read_json( myfile );
    myfile.close();
  };
  // ************************************************************** World::str
  /** Dump avec string */
  std::string str_dump() const;
  /** Display avec string */
  std::string str_display() const;
  // *********************************************************** World::update
  void update( double delta_t );
  // ************************************************************* World::cell
  /** La Cell à la position donnée ou nullptr */
  CellPtr cell( const Vec2& pos ) const;
  // ******************************************************** World::attributs
public:
  unsigned int nb_row() const {return _nb_row;};
  unsigned int nb_col() const {return _nb_col;};
  double stime() const {return _stime;};
  std::vector<Wall> walls() const {return _l_wall;};
  const std::list<std::unique_ptr<Chuchu>>& chuchu() const {return _l_chuchu;};
  const CRocketPtr& rocket() const {return _l_rocket;};
  const CCellPtr& arrow() const {return _l_arrow;};
private:
  /** Taille du monde */
  unsigned int _nb_row, _nb_col;
  /** Temps de simulation */
  double _stime;
  /** Toutes les cases */
  CCellPtr _l_cell;
  /** Les murs */
  std::vector<Wall> _l_wall;
  /** Les Chuchu */
  std::list<std::unique_ptr<Chuchu>> _l_chuchu;
  /** Les Rockets */
  CRocketPtr _l_rocket;
  /** Les Sources */
  CSourcePtr _l_source;
  /** Les Arrow */
  CCellPtr _l_arrow;
private:
  // ********************************************************** World::private
  /** @return true si le chuchu est toujours vivant */
  bool set_cell( Chuchu& chu ) const;
  void read_json( std::istream& input_stream );
};
#endif // WORLD_CPP
