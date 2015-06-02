/* -*- coding: utf-8 -*- */

/** 
 * Monde de 3x4 case avec une Rocket
 */

#include <iostream>                       // std::cout

#include <world.hpp>

//******************************************************************************
int main( int argc, char *argv[] )
{
  std::cout << "__CREATION" << std::endl;
  World world( "data/world_3x4.json");
  std::cout << world.str_dump() << std::endl;

  std::cout << "__RUNNING" << "\n";
  for( unsigned int i = 0; i < 50; ++i) {
    world.update();
  }

  std::cout << "__END" << std::endl;
  std::cout << world.str_dump() << std::endl;


  return 0;
}
