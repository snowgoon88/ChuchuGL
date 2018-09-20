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
#include <stdexcept>      // std::out_of_range

#include <matrix2020/m_def.hpp>

namespace matrix2020 {

using PosList = std::list<Pos>;
using Index = struct s_Index { unsigned int col; unsigned int row; };  
// ***************************************************************************
// *************************************************************** Environment
// ***************************************************************************
class Environment
// TODO add copy, assigment as uses char** ressource
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
    dump << str_display();

    dump << str_walls() << std::endl;
    dump << str_cells() << std::endl;
    
    return dump.str();
  }
  std::string str_display() const
  {
    std::stringstream disp;
    disp << "ENV: " << _nb_row << "x" << _nb_col << std::endl;
    
    for( unsigned int i = _nb_row; i > 0; i -= 1 ) {
      disp << _env[i-1] << std::endl;
    }
    
    return disp.str();
  }
  std::string str_walls() const
  {
    std::stringstream wall;
    wall << "W: ";
    for( auto& w: _wall_list) {
      wall << w << " ";
    }

    return wall.str();
  }
  std::string str_cells() const
  {
    std::stringstream cell;
    cell << "C: ";
    for( auto& c: _cell_list) {
      cell << c << " ";
    }

    return cell.str();
  }
  // *************************************************** Environment::get_info
  bool is_cell( const Pos& pos ) const
  {
    Index ind = get_index( pos );
    return (_env[ind.row][ind.col] == '.' );
  }
  bool is_obstacle( const Pos& pos ) const
  {
    Index ind = get_index( pos );
    return (_env[ind.row][ind.col] == 'X' );
  }

  // ************************************************** Environment::attributs
  PosList get_wall_list() const { return _wall_list; }
  PosList get_cell_list() const { return _cell_list; }
  
  char** _env;
  unsigned int _nb_row, _nb_col;
  PosList _wall_list;
  PosList _cell_list;

private:
  // ************************************************* Environment::build_info
  /** Builds _wall_list, _cell_list */
  void build_info();
  // ************************************************** Environment::get_index
  Index get_index( const Pos& pos ) const
  {
    // check validity and convert
    if ((pos.x < 0) or (pos.x >= (int)_nb_col) or
        (pos.y < 0) or (pos.y >= (int)_nb_row)){
      throw std::out_of_range( "pos="+pos.str_display()+" is not valid" );
    }
    return Index( {(unsigned int) pos.x, (unsigned int) pos.y} );
  }
}; // class Environment
}; // namespace matrix2020


#endif // M_ENVIRONMENT_HPP
