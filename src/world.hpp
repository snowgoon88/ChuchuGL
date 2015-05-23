/* -*- coding: utf-8 -*- */
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
#include <vector>                         // std::vector
#include <list>                           // std::list
#include <memory>                         // std::unique_ptr
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
  void init3x4()
  {
    std::cout << "World.init3x4()" << "\n";
    _nb_row = 3;
    _nb_col = 4;
    // Crée et ajoute les Cell avec murs autours
    for( unsigned int row = 0; row < _nb_row; ++row) {
      for( unsigned int col = 0; col < _nb_col; ++col) {
	if( col == _nb_col-1 and row == _nb_row-1 ) {
	  std::unique_ptr<Cell> rocket(new Rocket({(TPos)col,(TPos)row} ));
	  rocket->add_wall( _dir_up );
	  rocket->add_wall( _dir_right );
	  _l_cell.push_back( std::move(rocket) );
	}
	else {
	  std::unique_ptr<Cell> cell(new Cell({(TPos)col,(TPos)row} ));
	  if( row == 0 ) cell->add_wall( _dir_down );
	  if( row == _nb_row-1 ) cell->add_wall( _dir_up );
	  if( col == 0 ) cell->add_wall( _dir_left );
	  if( col == _nb_col-1 ) cell->add_wall( _dir_right );
	  _l_cell.push_back( std::move(cell) );
	}
      }
    } 
    // Down arrow in cell(3,1)
    _l_cell[3+_nb_col*1]->set_arrow( &_dir_down );

    // Crée un ajoute les Chuchu
    Chuchu chu( {3,0}, &_dir_right, 1.0 );
    set_cell( chu );
    _l_chuchu.push_back( chu );
  };
  void init2x5()
  {
    std::cout << "World.init2x5()" << "\n";
    _nb_row = 2;
    _nb_col = 5;
    // Crée et ajoute les Cell avec murs autours
    for( unsigned int row = 0; row < _nb_row; ++row) {
      for( unsigned int col = 0; col < _nb_col; ++col) {
	std::unique_ptr<Cell> cell(new Cell({(TPos)col,(TPos)row} ));
	if( row == 0 ) cell->add_wall( _dir_down );
	if( row == _nb_row-1 ) cell->add_wall( _dir_up );
	if( col == 0 ) cell->add_wall( _dir_left );
	if( col == _nb_col-1 ) cell->add_wall( _dir_right );
	_l_cell.push_back( std::move(cell) );
      }
    } 
    // Down arrow in cell(3,1)
    _l_cell[3+_nb_col*1]->set_arrow( &_dir_down );

    // Crée un ajoute les Chuchu
    Chuchu chu1( {3,0}, &_dir_right, 1.0 );
    set_cell( chu1 );
    _l_chuchu.push_back( chu1 );
    Chuchu chu2( {3,0}, &_dir_left, 1.0 );
    set_cell( chu2 );
    _l_chuchu.push_back( chu2 );
  };    
  void init1x5()
  {
    std::cout << "World.init1x5()" << "\n";
    _nb_row = 1;
    _nb_col = 5;
    // Crée et ajoute les Cell
    for( unsigned int row = 0; row < _nb_row; ++row) {
      for( unsigned int col = 0; col < _nb_col; ++col) {
	_l_cell.push_back( std::unique_ptr<Cell>(new Cell( {(TPos)col,(TPos)row} )) );
      }
    }
    // Right arrow in cell(0,0)
    _l_cell[0]->set_arrow( &_dir_right );
    // Left arrow in cell(2,0)
    _l_cell[2]->set_arrow( &_dir_left );
    
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
       dump << cell->str_dump() << std::endl;
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
    for( auto it = _l_chuchu.begin(); it != _l_chuchu.end(); ++it ) {      
      // MAJ coordonnées Chuchu
      (*it).update( 0.3 );
      // MAJ Cell du Chuchu
      if( set_cell( *it ) == false ) {
	std::cout << "argllll CHUCHU dies"<< std::endl;;
	it = _l_chuchu.erase( it ); // Chuchu meurt et 'it' maj
	std::cout << "....... CHUCHU is dead";
	std::cout << " only " << _l_chuchu.size() << " left" << std::endl;
      }
    }
  };

    
private:
  // ***************************************************************** LINK CELL
  /** @return true si le chuchu est toujours vivant */
  bool set_cell( Chuchu& chu ) 
  {
    // Un monde torique
    if( chu.pos().x > _nb_col+1 ) {
      chu.set_pos( chu.pos().x - (_nb_col+1), chu.pos().y );
    }
    else if( chu.pos().x < 0 ) {
      chu.set_pos( chu.pos().x + (_nb_col+1), chu.pos().y );
    }
    if( chu.pos().y > _nb_row+1 ) {
      chu.set_pos( chu.pos().x, chu.pos().y - (_nb_row+1) );
    }
    else if( chu.pos().y < 0 ) {
      chu.set_pos( chu.pos().x,  chu.pos().y+ (_nb_row+1) );
    }
    // La position du Chuchu devient des coordonnées entière, et 
    // donc l'index de la Cell dans _l_cell
    unsigned int idx = (unsigned int) floor(chu.pos().x);
    unsigned int idy = (unsigned int) floor(chu.pos().y);
    // Implémente un monde torique
    std::cout << chu.str_dump() << " would be in cell(" << idx << "; "  << idy << ")" << std::endl;
    bool changed_cell = chu.set_cell( _l_cell[idx+_nb_col*idy].get() );
    if( changed_cell ) {
      // Que se passe-t-il en arrivant sur la Cell ?
      bool still_alive = _l_cell[idx+_nb_col*idy]->entered_by( chu );
      return still_alive;
    }
    return true; // still_alive
  };
  
  // ****************************************************************** VARIABLE
private:
  /** Taille du monde */
  unsigned int _nb_row, _nb_col;
  /** Toutes les cases */
  std::vector<std::unique_ptr<Cell>> _l_cell;
  /** Et les Chuchu */
  std::list<Chuchu> _l_chuchu;
};
#endif // WORLD_CPP
