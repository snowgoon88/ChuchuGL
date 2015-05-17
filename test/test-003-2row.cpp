/* -*- coding: utf-8 -*- */

/** 
 * Monde de 2x5 cases avec Murs.
 */

#include <iostream>                       // std::cout

#include <world.hpp>

//******************************************************************************
int main( int argc, char *argv[] )
{
  World world;
  world.init2x5();
  std::cout << world.str_dump() << std::endl;

  for( unsigned int i = 0; i < 50; ++i) {
    world.update();
  }

  return 0;
}
