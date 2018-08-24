/* -*- coding: utf-8 -*- */

#ifndef M_ENVIRONMENT_HPP
#define M_ENVIRONMENT_HPP

/** 
 * Load one "level" of the matrix, i.e: environement.
 * From .txt file (see data/matrix00.txt)
 *
 * Read 'X', '.'
 */

#include <iostream>       // std::cout
#include <sstream>                        // std::stringstream
#include <string.h>       // strcpy
#include <list>

namespace matrix2020 {

using Pos = struct s_Pos {
  unsigned int x;
  unsigned int y;
};
using PosList = std::list<Pos>;

// ***************************************************************************
// *************************************************************** Environment
// ***************************************************************************
class Environment
{
public:
  // *************************************************** Environment::creation
  Environment() :
    _env(nullptr), _nb_row(0), _nb_col(0)
  {
  }
  // **************************************************** Environment::destroy
  void free_env();
  ~Environment()
  {
    free_env();
  }
  // ********************************************** Environment::load_from_txt
  void load_from_txt( const std::string& filename );
  // ******************************************************** Environment::str
  std::string str_dump() const
  {
    std::stringstream dump;
    dump << "ENV: " << _nb_row << "x" << _nb_col << std::endl;
    
    for( unsigned int i = _nb_row; i > 0; i -= 1 ) {
      dump << _env[i-1] << std::endl;
    }

    dump << str_walls() << std::endl;
    dump << str_cells() << std::endl;
    
    return dump.str();
  }
  std::string str_walls() const
  {
    std::stringstream wall;
    wall << "W: ";
    for( auto& w: _wall_list) {
      wall << "(" << w.x << ", " << w.y << ") ";
    }

    return wall.str();
  }
  std::string str_cells() const
  {
    std::stringstream cell;
    cell << "C: ";
    for( auto& c: _cell_list) {
      cell << "(" << c.x << ", " << c.y << ") ";
    }

    return cell.str();
  }
  // ************************************************* Environment::build_info
  void build_info();

  // ************************************************** Environment::attributs
  PosList get_wall_list() const { return _wall_list; }
  PosList get_cell_list() const { return _cell_list; }
  
  char** _env;
  unsigned int _nb_row, _nb_col;
  PosList _wall_list;
  PosList _cell_list;
  
}; // class Environment
}; // namespace matrix2020


#endif // M_ENVIRONMENT_HPP
