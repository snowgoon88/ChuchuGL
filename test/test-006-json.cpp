/* -*- coding: utf-8 -*- */

/** 
 * test-006-json.cpp
 *
 * Reads a file with json-like World data.
 *
 */

#include <iostream>
#include <fstream>                    // std::ifstream

#include <world.hpp>


// ******************************************************************** Global
#define WORLD_FILE "../data/world_6x5.json"

//******************************************************************************
int main( int argc, char *argv[] )
{
  World world( "../data/world_6x5.json");
  std::cout << world.str_dump() << std::endl;

  return 0;
}
