/* -*- coding: utf-8 -*- */

/** 
 * Monde de 2x5 cases avec Murs.
 */

#include <iostream>                       // std::cout

#include <world.hpp>

//******************************************************************************
int main( int argc, char *argv[] )
{
  World world( "data/world_2x5.json" );
  std::cout << world.str_dump() << std::endl;

  for( unsigned int i = 0; i < 50; ++i) {
    world.update();
  }

  return 0;
}
