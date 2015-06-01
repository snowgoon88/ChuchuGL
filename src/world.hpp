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

#include <json_wrapper.hpp>               // Wrapper around rapidjson
//#include "rapidjson/writer.h"           // rapidjson
#include "rapidjson/document.h"           // rapidjson's DOM-style API

#include <cell.hpp>
#include <chuchu.hpp>

// ******************************************************************** GLOBAL
typedef struct {
  Pos x, y;
  unsigned int idd; // index direction
} Wall;
// ***************************************************************************
// ********************************************************************* WORLD
// ***************************************************************************
class World {
public:
  // ****************************************************************** CREATION
  World() {};
  void read_json( std::istream& input_stream )
  {
    // Wrapper pour lire document
    JSON::IStreamWrapper is(input_stream);
    // Parse into a document
    rapidjson::Document doc;
    doc.ParseStream( is );
    
    // Taille du monde
    _nb_row = doc["nb_row"].GetUint();
    _nb_col = doc["nb_col"].GetUint();
    std::cout << "Monde de " << _nb_row << " x " << _nb_col << " cases." << std::endl;
    _l_cell.clear();
    for( unsigned int row = 0; row < _nb_row; ++row) {
      for( unsigned int col = 0; col < _nb_col; ++col) {
	std::unique_ptr<Cell> cell(new Cell({(Pos)col,(Pos)row} ));
	_l_cell.push_back( std::move(cell) );
      }
    }
    
    // Les murs
    _l_wall.clear();
    rapidjson::Value& walls = doc["walls"];
    // Pour chaque mur
    for (rapidjson::SizeType i = 0; i < walls.Size(); i++) {
      //Un array de 3 var : x, y, dir
      auto&  w = walls[i];
      int x = w[0].GetInt();
      int y = w[1].GetInt();
      unsigned int dir = w[2].GetUint();
      std::cout << " Mur en (" << x << "; " << y << ")";
      std::cout << " dir=" <<  _dir_str(dir) << std::endl;
      _l_wall.push_back( {(Pos)x, (Pos)y, dir} );
      if( dir == 2 ) { // down
	_l_cell[x+_nb_col*y]->add_wall( _dir_down );
	// case du dessous
	y = y - 1;
	if( y < 0 ) { 
	  y = _nb_row-1;
	  // doit aussi ajouter un mur dans _l_wall car case de bord.
	  _l_wall.push_back( {(Pos)x, (Pos)y, _dir_up.index} );
	  std::cout << "Ajout mur UP" << std::endl;
	}
	  
	_l_cell[x+_nb_col*y]->add_wall( _dir_up );
      }
      else if( dir == 3 ) { // left
	_l_cell[x+_nb_col*y]->add_wall( _dir_left );
	// case de gauche
	x = x - 1;
	if( x < 0 ) {
	  x = _nb_col-1;
	  // doit aussi ajouter un mur dans _l_wall car case de bord
	  _l_wall.push_back( {(Pos)x, (Pos)y, _dir_right.index} );
	  std::cout << "Ajout mur RIGHT" << std::endl;
	}
	_l_cell[x+_nb_col*y]->add_wall( _dir_right );
      }
    }

    // Chuchu
    rapidjson::Value& chuchu = doc["chuchu"];
    // Pour chaque chuchu
    for (rapidjson::SizeType i = 0; i < chuchu.Size(); i++) {
      //Un array de 3 var : x, y, dir
      auto&  c = chuchu[i];
      int x = c[0].GetInt();
      int y = c[1].GetInt();
      unsigned int dir = c[2].GetUint();
      // Crée un ajoute les Chuchu
      Chuchu chu( {(Pos)x, (Pos)y}, &_l_dir[dir], 1.0 );
      set_cell( chu );
      _l_chuchu.push_back( chu );
    }

    // Rocket
    rapidjson::Value& rockets = doc["rocket"];
    // Pour chaque rocket
    for (rapidjson::SizeType i = 0; i < rockets.Size(); i++) {
      //Un array de 2 var : x, y
      auto&  r = chuchu[i];
      int x = r[0].GetInt();
      int y = r[1].GetInt();
      // Crée un ajoute les Rocket
      std::unique_ptr<Cell> rocket(new Rocket(*_l_cell[x+_nb_col*y]));
      _l_cell[x+_nb_col*y] = std::move(rocket);
    }
  };
  void init3x4()
  {
    std::cout << "World.init3x4()" << "\n";
    _nb_row = 3;
    _nb_col = 4;
    _l_wall.push_back( {0, 0, 2} );
    _l_wall.push_back( {0, 0, 3} );
    _l_wall.push_back( {0, 1, 3} );
    // Crée et ajoute les Cell avec murs autours
    for( unsigned int row = 0; row < _nb_row; ++row) {
      for( unsigned int col = 0; col < _nb_col; ++col) {
	if( col == _nb_col-1 and row == _nb_row-1 ) {
	  std::unique_ptr<Cell> rocket(new Rocket({(Pos)col,(Pos)row} ));
	  rocket->add_wall( _dir_up );
	  rocket->add_wall( _dir_right );
	  _l_cell.push_back( std::move(rocket) );
	}
	else {
	  std::unique_ptr<Cell> cell(new Cell({(Pos)col,(Pos)row} ));
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
	std::unique_ptr<Cell> cell(new Cell({(Pos)col,(Pos)row} ));
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
	_l_cell.push_back( std::unique_ptr<Cell>(new Cell( {(Pos)col,(Pos)row} )) );
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
  std::string str_dump() const
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
  bool set_cell( Chuchu& chu ) const
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
public:
  unsigned int nb_row() const {return _nb_col;};
  unsigned int nb_col() const {return _nb_col;};
  std::vector<Wall> walls() const {return _l_wall;};
  std::list<Chuchu> chuchu() const {return _l_chuchu;};
private:
  /** Taille du monde */
  unsigned int _nb_row, _nb_col;
  /** Toutes les cases */
  std::vector<std::unique_ptr<Cell>> _l_cell;
  /** Les murs */
  std::vector<Wall> _l_wall;
  /** Et les Chuchu */
  std::list<Chuchu> _l_chuchu;
};
#endif // WORLD_CPP
