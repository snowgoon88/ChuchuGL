/* -*- coding: utf-8 -*- */

/** 
 * Implement turningg
 */
#include <m_def.hpp>
#include <math.h>
#include <iostream>

namespace matrix2020 {

unsigned int get_dir_index( const Pos& dir )
{
  for( unsigned int i = 0; i < AllDir.size(); ++i) {
    if (AllDir[i] == dir) {
      return i;
    }
  }
  throw std::out_of_range( dir.str_display()+" is not valid dir" );
  return 0;
}
Pos turn_right( const Pos& dir )
{
  std::cout <<  ((get_dir_index(dir) + 1) % AllDir.size()) << std::endl;
  return AllDir[(get_dir_index(dir) + 1) % AllDir.size()];
}
Pos turn_left( const Pos& dir )
{
  return AllDir[(get_dir_index(dir) - 1) % AllDir.size()];
}
float angle_from_dir( const Pos& dir )
{
  return ((get_dir_index(dir) - 1) % AllDir.size()) * (-M_PI/2.0);
}
};  // matrix2020
