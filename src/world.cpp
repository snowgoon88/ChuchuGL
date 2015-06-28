/* -*- coding: utf-8 -*- */

#include <world.hpp>
#include <cell.hpp>
#include <chuchu.hpp>

#include <math.h>                         // floor, 
#include <json_wrapper.hpp>               // Wrapper around rapidjson
//#include "rapidjson/writer.h"           // rapidjson
#include <rapidjson/document.h>           // rapidjson's DOM-style API

// **************************************************************** World::str
/** Dump avec string */
std::string World::str_dump() const
{
  std::stringstream dump;
  
  dump << "__WORLD__ " << _nb_row << " x " << _nb_col;
  dump << " time=" << stime() << std::endl;
  for( auto& cell : _l_cell ) {
    dump << cell->str_dump() << std::endl;
  }
  for( auto& chu : _l_chuchu ) {
    dump << "  " << chu->str_dump() << std::endl;
  }
  
  return dump.str();
}
/** Display avec string */
std::string World::str_display() const
{
  std::stringstream str;
  
  for( auto& chu : _l_chuchu ) {
    str << "  " << chu->str_dump() << std::endl;
  }
  
  return str.str();
}
// ************************************************************* World::update
void World::update( double delta_t )
{
  // maj du temps
  _stime += delta_t;
  // Fait le tour des sources pour créer les Chuchu
  for( auto& source: _l_source) {
    std::unique_ptr<Chuchu> chu = source->update();
    if( chu ) {
      _l_chuchu.push_back( std::move(chu) );
    }
  }
  // Calcule la nouvelle position de chaque Chuchu,
  // puis lui attribue sa Cell, ce qui permet de savoir
  // par où il va sortir.
  for( auto it = _l_chuchu.begin(); it != _l_chuchu.end(); ++it ) {      
    // MAJ coordonnées Chuchu
    (*it)->update( delta_t );
    // MAJ Cell du Chuchu
    if( set_cell( **it ) == false ) {
      std::cout << "argllll CHUCHU dies"<< std::endl;
      it = _l_chuchu.erase( it ); // Chuchu meurt et 'it' maj
      std::cout << "....... CHUCHU is dead";
      std::cout << " only " << _l_chuchu.size() << " left" << std::endl;
    }
  }
}
// *************************************************************** World::cell
CellPtr World::cell( const Vec2& pos ) const
{
  // Si pas dans le World => nullptr
  if( pos.x < 0 or pos.x > _nb_col ) return nullptr;
  if( pos.y < 0 or pos.y > _nb_row ) return nullptr;
  
  // La position est transformée en coordonnées entière, et 
  // donc l'index de la Cell dans _l_cell
  unsigned int idx = (unsigned int) floor(pos.x);
  unsigned int idy = (unsigned int) floor(pos.y);
  
  return _l_cell[idx+_nb_col*idy];
}
// *********************************************************** World::set_cell
/** @return true si le chuchu est toujours vivant */
bool World::set_cell( Chuchu& chu ) const
{
  // Un monde torique
  if( chu.pos().x > _nb_col ) {
    chu.set_pos( chu.pos().x - (_nb_col), chu.pos().y );
  }
  else if( chu.pos().x < 0 ) {
    chu.set_pos( chu.pos().x + (_nb_col), chu.pos().y );
  }
  if( chu.pos().y > _nb_row ) {
    chu.set_pos( chu.pos().x, chu.pos().y - (_nb_row) );
  }
  else if( chu.pos().y < 0 ) {
    chu.set_pos( chu.pos().x, chu.pos().y + (_nb_row) );
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
}
// ********************************************************** World::read_json
void World::read_json( std::istream& input_stream )
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
      CellPtr cell(new Cell({(Pos)col,(Pos)row} ));
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
  _l_chuchu.clear();
  rapidjson::Value& chuchu = doc["chuchu"];
  // Pour chaque chuchu
  for (rapidjson::SizeType i = 0; i < chuchu.Size(); i++) {
    //Un array de 3 var : x, y, dir
    auto&  c = chuchu[i];
    int x = c[0].GetInt();
    int y = c[1].GetInt();
    unsigned int dir = c[2].GetUint();
    // Crée un ajoute les Chuchu
    std::cout << "add CHUCHU at (" << x << "; " << y << ")" << std::endl;
    std::unique_ptr<Chuchu> chu(new Chuchu({(Pos)x+0.5, (Pos)y+0.5}, &_l_dir[dir], 1.0 ));
    set_cell( *chu );
    _l_chuchu.push_back( std::move(chu) );
  }
  
  // Rocket
  _l_rocket.clear();
  rapidjson::Value& rockets = doc["rocket"];
  // Pour chaque rocket
  for (rapidjson::SizeType i = 0; i < rockets.Size(); i++) {
    //Un array de 2 var : x, y
    auto&  r = rockets[i];
    int x = r[0].GetInt();
    int y = r[1].GetInt();
    // Crée un ajoute les Rocket
    RocketPtr rocket(new Rocket(*this, *_l_cell[x+_nb_col*y]));
    _l_rocket.push_back( rocket );
    _l_cell[x+_nb_col*y] = rocket;
  }
  // Source
  _l_source.clear();
  rapidjson::Value& sources = doc["source"];
  // Pour chaque source
  for (rapidjson::SizeType i = 0; i < sources.Size(); i++) {
    //Un array de 3 var : x, y, dir
    auto&  s = sources[i];
    int x = s[0].GetInt();
    int y = s[1].GetInt();
    unsigned int dir = s[2].GetUint();
    std::cout << "Crée SOURCE en " << x <<"; "<<y<<"; d="<<dir << std::endl;
    // Crée et ajoute les Sources
    SourcePtr source(new Source(*this, *_l_cell[x+_nb_col*y], &_l_dir[dir]));
    _l_source.push_back( source );
    _l_cell[x+_nb_col*y] = source;
  }
  
  // Arrow
  // _l_arrow.clear();
  // rapidjson::Value& arrows = doc["arrow"];
  // // Pour chaque arrow
  // for (rapidjson::SizeType i = 0; i < arrows.Size(); i++) {
  //   //Un array de 3 var : x, y, dir
  //   auto&  a = arrows[i];
  //   int x = a[0].GetInt();
  //   int y = a[1].GetInt();
  //   unsigned int dir = a[2].GetUint();
  //   // Ajoute les Arrow
  //   _l_cell[x+_nb_col*y]->set_arrow( &_l_dir[dir]);
  //   _l_arrow.push_back( _l_cell[x+_nb_col*y] );
  //   // TODO add/remove arrow ?
  // }
}
