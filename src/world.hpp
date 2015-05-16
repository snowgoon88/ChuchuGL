/* -*- coding: utf-8 -*- */

#ifndef WORLD_HPP
#define WORLD_HPP

/** 
 * Le World est composé de Cases et de Chuchu.
 *
 * Actuellement _l_cell[idx+idy*NB_COL] est en (idx,idy)
 * - 5 case de (0,0) à (5,0)
 * - flèche_gauche dans (0,0)
 * - flèche_droite dans (2,0)
 */

#include <sstream>                        // std::stringstream
#include <vector>                         // std::vector
#include <math.h>                         // floor, 

#include <global.hpp>

#include <cell.hpp>
#include <chuchu.hpp>

// ******************************************************************** GLOBAL
#define NB_COL 5
#define NB_ROW 1
// ***************************************************************************
// ********************************************************************* WORLD
// ***************************************************************************
class World {
public:
  // ****************************************************************** CREATION
  World() 
  {
    // Crée et ajoute les Cell
    for( unsigned int row = 0; row < NB_ROW; ++row) {
      for( unsigned int col = 0; col < NB_COL; ++col) {
	_l_cell.push_back( Cell( {(TPos)col,(TPos)row} ));
      }
    }
    // Right arrow in cell(0,0)
    _l_cell[0].set_arrow( &dir_right );
    // Left arrow in cell(2,0)
    _l_cell[2].set_arrow( &dir_left );
    
    // Crée un ajoute les Chuchu
    Chuchu chu( {0,0}, &dir_right, 1.0 );
    set_cell( chu );
    _l_chuchu.push_back( chu );
  };

  // *********************************************************************** STR
  /** Dump avec string */
  std::string str_dump()
  {
    std::stringstream dump;
    
    dump << "__WORLD__" << std::endl;
    for( auto& cell : _l_cell ) {
      dump << cell.str_dump() << std::endl;
    }
    for( auto& chu : _l_chuchu ) {
      dump << "  " << chu.str_dump() << std::endl;
    }

    return dump.str();
  };

  // ******************************************************************** UPDATE
  void update()
  {
    // Calcule la nouvelle position de chaque Chuchu,
    // puis lui attribue sa Cell, ce qui permet de savoir
    // par où il va sortir.
    for( auto& chu : _l_chuchu ) {
      chu.update( 0.3 );
      set_cell( chu );
    }
  };

    
private:
  // ***************************************************************** LINK CELL
  void set_cell( Chuchu& chu ) 
  {
    // La position du Chuchu devient des coordonnées entière, et 
    // donc l'index de la Cell dans _l_cell
    unsigned int idx = (unsigned int) floor(chu.pos().x);
    unsigned int idy = (unsigned int) floor(chu.pos().y);
    std::cout << chu.str_dump() << " would be in cell(" << idx << "; "  << idy << ")" << std::endl;
    chu.set_cell( &_l_cell[idx+NB_COL*idy] );
  };
  
  // ****************************************************************** VARIABLE
private:
  /** Toutes les cases */
  std::vector<Cell> _l_cell;
  /** Et les Chuchu */
  std::vector<Chuchu> _l_chuchu;
};
#endif // WORLD_HPP
