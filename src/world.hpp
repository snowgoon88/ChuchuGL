/* -*- coding: utf-8 -*- */

#pragma once

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
#include <vector>                         // std::vector
#include <math.h>                         // floor, 

#include <global.hpp>

#include <cell.hpp>
#include <chuchu.hpp>

// ******************************************************************** GLOBAL

// ***************************************************************************
// ********************************************************************* WORLD
// ***************************************************************************
class World {
public:
  // ****************************************************************** CREATION
  World() {};
  void init2x5()
  {
    _nb_row = 2;
    _nb_col = 5;
    // Crée et ajoute les Cell avec murs autours
    for( unsigned int row = 0; row < _nb_row; ++row) {
      for( unsigned int col = 0; col < _nb_col; ++col) {
	Cell cell({(TPos)col,(TPos)row} );
	if( row == 0 ) cell.add_wall( _dir_down );
	if( row == _nb_row-1 ) cell.add_wall( _dir_up );
	if( col == 0 ) cell.add_wall( _dir_left );
	if( col == _nb_col-1 ) cell.add_wall( _dir_right );
	_l_cell.push_back( cell );
      }
    } 
    // Down arrow in cell(3,1)
    _l_cell[3+_nb_col*1].set_arrow( &_dir_down );

    // Crée un ajoute les Chuchu
    Chuchu chu( {3,0}, &_dir_right, 1.0 );
    set_cell( chu );
    _l_chuchu.push_back( chu );
  };    
  void init1x5()
  {
    _nb_row = 1;
    _nb_col = 5;
    // Crée et ajoute les Cell
    for( unsigned int row = 0; row < _nb_row; ++row) {
      for( unsigned int col = 0; col < _nb_col; ++col) {
	_l_cell.push_back( Cell( {(TPos)col,(TPos)row} ));
      }
    }
    // Right arrow in cell(0,0)
    _l_cell[0].set_arrow( &_dir_right );
    // Left arrow in cell(2,0)
    _l_cell[2].set_arrow( &_dir_left );
    
    // Crée un ajoute les Chuchu
    Chuchu chu( {0,0}, &_dir_right, 1.0 );
    set_cell( chu );
    _l_chuchu.push_back( chu );
  };

  // *********************************************************************** STR
  /** Dump avec string */
  std::string str_dump()
  {
    std::stringstream dump;
    
    dump << "__WORLD__ " << _nb_row << " x " << _nb_col << std::endl;
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
    chu.set_cell( &_l_cell[idx+_nb_col*idy] );
  };
  
  // ****************************************************************** VARIABLE
private:
  /** Taille du monde */
  unsigned int _nb_row, _nb_col;
  /** Toutes les cases */
  std::vector<Cell> _l_cell;
  /** Et les Chuchu */
  std::vector<Chuchu> _l_chuchu;
};
