/* -*- coding: utf-8 -*- */

#include <matrix2020/m_environment.hpp>
#include <fstream>

// ************************************************ Environment::load_from_txt
void matrix2020::Environment::load_from_txt( const std::string& filename )
{
  // stream
  std::ifstream is( filename );
  std::string line;
  bool not_finished = true;
  
  // free previous environment
  free_env();
  
  // read nb_row nb_col
  while( not_finished ) { 
    std::getline( is, line);
    std::cout << "DEB-" << line << std::endl;
    // Ignore if begin with '#'
    if( line.front() != '#' ) {
      std::istringstream iss(line);
      iss >> _nb_row >> _nb_col;
      not_finished = false;
    }
  }
  
  // organise memory
  _env = new char*[_nb_row];
  // read environment data
  not_finished = true;
  unsigned int row = 0;
  while( not_finished ) {
    std::getline( is, line);
    // Ignore if begin with '#'
    if( line.front() != '#' ) {
      _env[_nb_row-(row+1)] = new char[_nb_col+1];
      strcpy( _env[_nb_row-(row+1)], line.c_str() );
      row++;
      if( row >= _nb_row ) not_finished = false;
    }
  }
  
  // and build info on what is in the environment
  build_info();
}
// *************************************************** Environment::build_info
void matrix2020::Environment::build_info()
{
  // a list of wall ('X') position
  _wall_list.clear();
  // a list of cell ('.') position
  _cell_list.clear();
  
  for( unsigned int row = 0; row < _nb_row; ++row) {
    for( unsigned int col = 0; col < _nb_col; ++col) {
      if( _env[row][col] == 'X' ) {
        _wall_list.push_back( {col,row} );
      }
      else if( _env[row][col] == '.' ) {
        _cell_list.push_back( {col,row} );
      }
    }
  }
}
// ***************************************************** Environment::free_env
void matrix2020::Environment::free_env()
{
  if( _env != nullptr ) {
    for( unsigned int i = 0; i < _nb_row; ++i) {
      delete _env[i];
    }
    delete _env;
  }
  _nb_row = 0; _nb_col = 0;
}
