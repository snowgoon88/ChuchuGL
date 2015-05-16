/* -*- coding: utf-8 -*- */

/** 
 * Monde de 5 cases.
 */

#include <iostream>                       // std::cout

#include <world.hpp>

//******************************************************************************
int main( int argc, char *argv[] )
{
  World world;
  std::cout << world.str_dump() << std::endl;

  for( unsigned int i = 0; i < 20; ++i) {
    world.update();
  }

  return 0;
}
