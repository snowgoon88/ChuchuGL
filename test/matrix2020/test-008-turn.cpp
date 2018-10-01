/* -*- coding: utf-8 -*- */

/** 
 * Can we use get or find in std::vector
 */

#include <iostream>
#include <matrix2020/m_def.hpp>
using namespace matrix2020;

int main(int argc, char *argv[])
{
  Pos pos = D_RIGHT;
  std::cout << "START:" << pos << std::endl;
  std::cout << "R:" <<  turn_right(pos) << std::endl;
  std::cout << "RR:" <<  turn_right(turn_right(pos)) << std::endl;
  std::cout << "RRR:" <<  turn_right(turn_right(turn_right(pos))) << std::endl;
  std::cout << "RRRR:" <<  turn_right(turn_right(turn_right(turn_right(pos)))) << std::endl;
  std::cout << "=======" << std::endl;
  std::cout << "START: " << pos << std::endl;
  std::cout << "L:" <<  turn_left(pos) << std::endl;
  std::cout << "LL:" <<  turn_left(turn_left(pos)) << std::endl;
  std::cout << "LLL:" <<  turn_left(turn_left(turn_left(pos))) << std::endl;
  std::cout << "LLLL:" <<  turn_left(turn_left(turn_left(turn_left(pos)))) << std::endl;
  
  return 0;
}
