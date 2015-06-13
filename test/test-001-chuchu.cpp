/* -*- coding: utf-8 -*- */

/** 
 * Création d'un Chuchu et affichage.
 * Création d'une Cell, copie et Rocket à partir de la Cell.
 */

#include <iostream>                       // std::cout

#include <chuchu.hpp>

//******************************************************************************
int main( int argc, char *argv[] )
{
  Chuchu chu;
  std::cout << chu.str_dump() << std::endl;

  // Une Cell, copie et transforme en Rocket
  Cell cell( {1,2} );
  cell.add_wall( _dir_up );
  cell.set_arrow( &_dir_right );
  std::cout << cell.str_dump() << std::endl;

  Cell other(cell);
  std::cout << other.str_dump() << std::endl;

  // On en a besoin dans la création du monde : Rocket à partir de Cell
  Rocket rocket(cell);
  std::cout << rocket.str_dump() << std::endl;
  return 0;
}
