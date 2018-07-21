/* -*- coding: utf-8 -*- */

#ifndef M_ENVIRONMENT_HPP
#define M_ENVIRONMENT_HPP

/** 
 * Load one "level" of the matrix, i.e: environement.
 * From .txt file (see data/matrix00.txt)
 */

#include <iostream>       // std::cout
#include <fstream>
#include <sstream>                        // std::stringstream
#include <string.h>       // strcpy

namespace matrix2020 {
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
  void free_env()
  {
    if( _env != nullptr ) {
      for( unsigned int i = 0; i < _nb_row; ++i) {
        delete _env[i];
      }
      delete _env;
    }
    _nb_row = 0; _nb_col = 0;
  }
  ~Environment()
  {
    free_env();
  }
  // ********************************************** Environment::load_from_txt
  void load_from_txt( const std::string& filename )
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
    // read envorinment data
    not_finished = true;
    unsigned int row = 0;
    while( not_finished ) {
      std::getline( is, line);
      // Ignore if begin with '#'
      if( line.front() != '#' ) {
        _env[row] = new char[_nb_col+1];
        strcpy( _env[row], line.c_str() );
        row++;
        if( row >= _nb_row ) not_finished = false;
      }
    }
  }
  // ******************************************************** Environment::str
  std::string str_dump() const
  {
    std::stringstream dump;
    dump << "ENV: " << _nb_row << "x" << _nb_col << std::endl;
    
    for( unsigned int i = 0; i < _nb_row; ++i) {
      dump << _env[i] << std::endl;
    }

    return dump.str();
  }
  // ************************************************** Environment::attributs
  char** _env;
  unsigned int _nb_row, _nb_col;
  
}; // class Environment
}; // namespace matrix2020


#endif // M_ENVIRONMENT_HPP
